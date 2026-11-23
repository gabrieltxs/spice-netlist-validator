# Parser_Spice

Validador de sintaxe de netlists SPICE em C++. O programa lê um arquivo de circuito, separa os tokens e aplica verificações específicas para componentes e diretivas, imprimindo diagnósticos com referências à linha de entrada.

**Este projeto não é um simulador elétrico:** não monta matrizes, calcula tensões/correntes nem executa análises AC/DC/transitórias. É uma implementação acadêmica de análise léxica e sintática, com uma gramática parcial.

## Compilar e executar

Requisitos: compilador C++11 (GCC ou Clang) e GNU Make. Python 3 é necessário apenas para executar os testes. Não há dependências externas de C++.

```sh
make
./build/Parser_Spice examples/rc.ckt
# Formas alternativas, mantidas para compatibilidade:
./build/Parser_Spice -f examples/rc.ckt
./build/Parser_Spice --filename examples/rc.ckt
make test
```

Sem Make, a compilação equivalente é:

```sh
c++ -std=c++11 -Wall -Wextra -pedantic -Iinclude src/main.cpp src/Circuito.cpp src/parser.cpp -o Parser_Spice
```

O primeiro registro do arquivo é sempre tratado como título. Use nomes de elementos e diretivas em maiúsculas; linhas de comentário começam com `*`, opcionalmente após espaços. O arquivo deve terminar com `.END`. Veja o exemplo RC incluído.

O programa imprime o título e eventuais diagnósticos. Erros de argumentos e de abertura do arquivo retornam código 1; ajuda retorna 0. **Os diagnósticos de sintaxe herdados ainda não produzem um código de saída de falha consistente**: não use apenas `$?` como certificado de validade do circuito.

## Organização

```text
src/             entrada CLI, tokenização/despacho e regras de validação
include/         declarações das classes e adaptação de cores do console
examples/        netlist pequeno, sem dados externos
tests/          testes de regressão da CLI (biblioteca padrão Python)
docs/            arquitetura, limitações e alterações de comportamento
Makefile         compilação e execução dos testes
```

## Escopo das regras

O despacho contém verificações para `R`, `C`, `L`, fontes `V/I`, `G/E`, `F/H`, `D`, `Q`, `J`, `M`, chamadas `X` e diretivas `.SUBCKT`, `.ENDS`, `.END`, `.OPTIONS`, `.NODESET`, `.IC`, `.AC`, `.DC`, `.PZ`, `.SENS`, `.TF`, `.TRAN` e `.PRINT`. A presença de um handler **não significa suporte completo ao padrão SPICE**. `.SAVE` é ignorado.

Os validadores de números, unidades, quantidade de argumentos e nós têm limitações históricas; expressões arbitrárias, modelos `.MODEL`, arquivos `.INCLUDE`, continuação de linhas com `+` e compatibilidade com todos os dialetos não são oferecidos. A checagem `.PRINT` é parcial, embora agora termine e use índices de tokens corretos. Veja [arquitetura e limitações](docs/ARCHITECTURE.md).

## Testes e manutenção

`make test` cobre os formatos da CLI, arquivos inexistentes, linhas vazias, CRLF, comentários, pontuação, terminação de `.PRINT`, parênteses, ausência de `.END` e referências de linha. O timeout de cada processo também detecta regressões que provoquem loops infinitos.

As regras históricas são mantidas em `src/parser.cpp`. Correções da gramática devem incluir exemplos válidos e inválidos e registrar mudanças na saída. Não substitua este projeto por um simulador SPICE na mesma alteração.

Licença MIT, conforme o cabeçalho original, reproduzida em [LICENSE](LICENSE).
