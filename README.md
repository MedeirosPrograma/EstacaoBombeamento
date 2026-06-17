# mateus medeiros

## 🔄 Fluxo de Funcionamento do Ecossistema

1. **Inicialização:** A `EstacaoBombeamento` abre o arquivo JSON de configuração inicial, despacha a criação dos sensores para a `SensorFactory`, aloca as bombas em memória e parametriza a planta.
2. **Ciclo Físico-Lógico (`processarCiclo`):**
   * O sistema valida as travas de segurança (sensores ativos e status de manutenção humana).
   * O `Tanque` calcula o impacto físico das bombas ativas (vazão, alteração de nível e acúmulo de pressão).
   * Os sensores atualizam suas leituras baseados no novo estado do fluido do tanque.
   * A central de inteligência processa as regras de negócio (revezamento de grupos por desgaste material ou acionamento de força máxima/dupla bomba em caso de níveis críticos).
3. **Persistência:** Os dados consolidados do ciclo são injetados diretamente no arquivo `historico_estacao.csv`.
4. **Inteligência de Negócios:** O arquivo Python (`.py`) consome o CSV gerado utilizando a biblioteca **Pandas** para auditoria de falhas, cálculo de médias operacionais e geração de relatórios de desempenho.

### 1. Padrão de Projeto: Factory (Fábrica de Sensores)

Para o gerenciamento e inicialização dos dispositivos de telemetria, foi implementado o padrão **SensorFactory**.

* **Por que essa escolha?** A estação inicializa sua rotina lendo um arquivo de configuração estática. O padrão Factory permite o desacoplamento total entre a classe principal (`EstacaoBombeamento`) e as implementações específicas de hardware (sensores de Nível, Vazão, Pressão, Contaminação).
* **Benefício:** Facilidade de expansão. Adicionar um novo tipo de sensor ao sistema exige apenas atualizar a fábrica, sem modificar a lógica central do ciclo de bombeamento.

### 2. Persistência de Dados: CSV em vez de SQL (MySQL)

Para o armazenamento do histórico de telemetria e eventos da planta, optou-se pela geração de arquivos **CSV (Comma-Separated Values)** em detrimento de um Banco de Dados Relacional (SQL).

* **Por que essa escolha?** * **Zero Infraestrutura:** O sistema dispensa dependências complexas de rede, contêineres Docker ou instalação de servidores locais de banco de dados, rodando de forma standalone através da biblioteca nativa `<fstream>`.
* **Eficiência de Memória:** O método de persistência grava os dados diretamente no disco via *append* (`std::ios::app`) ao final de cada ciclo. Isso elimina o acúmulo de logs na memória RAM, mitigando o risco de falhas por *Out of Memory* em regimes de operação contínua (24/7).
* **Sinergia com Data Science:** Arquivos CSV estruturados com delimitador ponto e vírgula (`;`) são o padrão da indústria para ingestão rápida e análise em ferramentas de inteligência.
