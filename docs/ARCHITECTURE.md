# Arquitetura e limites

## Fluxo

1. `src/main.cpp` interpreta um caminho posicional ou `-f`/`--filename` usando somente a biblioteca padrão.
2. `Circuito::runFile` abre o arquivo, reinicializa o estado e imprime a primeira linha como título.
3. `Circuito::setLine_from_ckt` ignora linhas vazias/comentários, separa palavras, `=` e `,`, verifica o balanceamento dos parênteses e constrói o mapa legado `token0`, `token1`, etc. Parênteses delimitam tokens, mas não são enviados aos handlers.
4. `Circuito::parseCircuit` faz um único despacho ao handler correspondente. O estado de `.SUBCKT`/`.ENDS` e `.END` pertence ao circuito.
5. `parser` verifica argumentos e imprime diagnósticos. `Console.h` preserva cores em Windows e usa texto simples nas outras plataformas.

## O que esta revisão mudou

A revisão original tinha apenas três arquivos `.cpp`: `Circuito.h` e as declarações de `parser` estavam ausentes, além de não haver configuração de build ou testes. Por isso não existe um executável de referência compilável nesta versão histórica para comparação ponta a ponta.

- Declarações reconstruídas a partir das definições e estado explicitamente inicializado.
- Fontes organizadas em `src/`, headers em `include/`, exemplo e testes em diretórios próprios.
- Remoção da dependência não fornecida `cxxopts` e do uso incondicional de Windows; a CLI aceita as opções documentadas e um caminho posicional.
- Tokenização sem `goto` e sem leituras além do tamanho da linha. Espaços, CRLF, `=`, `,` e parênteses são tratados em um único percurso.
- Comentários são ignorados antes de verificar parênteses; diagnósticos contam as linhas físicas incluindo o título.
- Despacho executado uma única vez por linha; a chamada duplicada podia alterar o estado de subcircuitos ao imprimir erros.
- Arquivo inexistente causa erro explícito com código de saída 1, em vez de aparentar uma análise normal.
- Comparações de tipos de fontes corrigidas de atribuição (`=`) para igualdade (`==`), preservando a identificação de `V/I`, `G/E` e `F/H` nos diagnósticos.
- `.PRINT` passou a incrementar o índice do loop e construir chaves com `std::to_string`, eliminando loop infinito e aritmética em literais de string.
- Construção da seta de diagnóstico limitada à linha quando um token não é encontrado.

## Limitações preservadas

Não houve reimplementação da gramática inteira. Os validadores numéricos reconhecem apenas um conjunto restrito de caracteres/sufixos; algumas cardinalidades de argumentos são permissivas. A validade de modelos, referências de componentes, subcircuitos não fechados, unidades e expressões não é garantida. Nomes e diretivas permanecem sensíveis a maiúsculas/minúsculas.

Muitos handlers imprimem erros mas retornam `ok`; `runFile` captura erros de parênteses para exibi-los. Portanto, um processo terminado com código 0 não garante que todos os diagnósticos de sintaxe estejam ausentes. Um futuro trabalho separado pode definir um resultado estruturado e unificar os códigos de saída, com regressões para todas as regras.

O teste automatizado cobre o conjunto listado no README, não conformidade completa com SPICE. Não há cálculo elétrico, resultado de simulação ou alegação de compatibilidade integral com ngspice/LTspice.
