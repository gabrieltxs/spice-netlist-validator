"""CLI regression checks; only Python's standard library is required."""
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

BINARY = str(Path(sys.argv.pop(1)).resolve())

class CliTests(unittest.TestCase):
    def run_program(self, *args):
        return subprocess.run([BINARY, *args], capture_output=True, text=True, timeout=3)

    def netlist(self, body, option=None):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'circuit with spaces.ckt'
            path.write_text('Test circuit\n' + body)
            return self.run_program(*([option] if option else []), str(path))

    def test_cli_forms(self):
        for flag in [None, '-f', '--filename']:
            result = self.netlist('V1 1 0 5\nR1 1 0 1k\n.END\n', flag)
            self.assertEqual(result.returncode, 0)
            self.assertEqual(result.stdout, 'Test circuit\n')

    def test_usage_and_missing_file(self):
        self.assertEqual(self.run_program('--help').returncode, 0)
        self.assertNotEqual(self.run_program().returncode, 0)
        self.assertNotEqual(self.run_program('-f').returncode, 0)
        self.assertNotEqual(self.run_program('/no-such-directory/missing.ckt').returncode, 0)

    def test_blank_lines_comments_and_crlf(self):
        result = self.netlist('\r\n  * ignored (comment\r\nR1\t1\t0\t1k\r\n.END\r\n')
        self.assertEqual(result.stdout, 'Test circuit\n')

    def test_punctuation_and_print_terminate(self):
        result = self.netlist('C1 1 0 1u IC=0\n.PRINT TRAN V(1) I(V1)\n.END\n')
        self.assertEqual(result.stdout, 'Test circuit\n')

    def test_unknown_element_reports_physical_line(self):
        result = self.netlist('\nZ1 1 0 4\n.END\n')
        self.assertIn('(path_UNDEFINED) line 3', result.stdout)

    def test_parentheses_and_missing_end(self):
        for text, diagnostic in [('C1 1 0 1u IC=(0\n', "unmatched '('"), ('R1 1 0 1k)\n', "unmatched ')'"), ('R1 1 0 1k\n', "Missing argument '.END'")]:
            self.assertIn(diagnostic, self.netlist(text).stdout)

    def test_invalid_source_reports_voltage_form(self):
        result = self.netlist('V1 1 0 invalid\n.END\n')
        self.assertIn('General form: VXXXXXXX', result.stdout)

if __name__ == '__main__':
    unittest.main()
