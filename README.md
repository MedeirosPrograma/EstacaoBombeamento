# Simulador de Estação de Bombeamento e Tanques (POO)

## Descrição do Projeto
Este projeto consiste em um simulador dinâmico de telemetria e física para uma **Estação de Bombeamento de Fluidos**, desenvolvido como parte dos requisitos práticos da disciplina de Programação Orientada a Objetos (POO). 

O sistema simula em tempo real o comportamento físico de tanques (nível, pressão, contaminação) sob a ação de múltiplos tipos de bombas acionadas por algoritmos de controle, coletando dados de múltiplos sensores gerados dinamicamente via fábrica de objetos.

---

## Fluxo de Funcionamento do Ecossistema

1. **Inicialização:** A `EstacaoBombeamento` abre o arquivo JSON de configuração inicial, despacha a criação dos sensores para a `SensorFactory`, aloca as bombas em memória e parametriza a planta.
2. **Ciclo Físico-Lógico (`processarCiclo`):**
   * O sistema valida as travas de segurança (sensores ativos e status de manutenção humana).
   * O `Tanque` calcula o impacto físico das bombas ativas (vazão, alteração de nível e acúmulo de pressão).
   * Os sensores atualizam suas leituras baseados no novo estado do fluido do tanque.
   * A central de inteligência processa as regras de negócio (revezamento de grupos por desgaste material ou acionamento de força máxima/dupla bomba em caso de níveis críticos).
3. **Persistência:** Os dados consolidados do ciclo são injetados diretamente no arquivo `historico_estacao.csv`.
4. **Inteligência de Negócios:** O arquivo Python (`.py`) consome o CSV gerado utilizando a biblioteca **Pandas** para auditoria de falhas, cálculo de médias operacionais e geração de relatórios de desempenho.

---

## Arquitetura em Duas Camadas (2-Tier Architecture)
O software foi desacoplado seguindo o modelo de arquitetura em duas camadas para separar estritamente a computação física e lógica de controle da camada de apresentação visual:

1. **Back-End (Camada de Processamento / Core C++):** Responsável por rodar o loop de simulação física de fluidos (`Tanque`), instanciar os atuadores (`Bomba`), processar as regras de telemetria na central (`EstacaoBombeamento`) e persistir os resultados brutos em arquivos estruturados.
2. **Front-End (Camada de Visão / Interface Streamlit Python):** Uma aplicação web leve que consome os dados persistidos pelo motor em C++, gerando dashboards interativos com gráficos temporais, indicadores de falha de hardware e relatórios analíticos para o operador técnico.

---

## Diagrama de Classes
O relacionamento entre os componentes do motor em C++ segue a modelagem orientada a objetos abaixo:

```text
       +---------------------+
       |  EstacaoBombeamento |
       +---------------------+
                  |
         +--------+            
         | 1                           
  +------v------+               
  |   Tanque    |---------------+ 
  +-------------+               | 1..*
         |               +------v------+
         | 1..*          |    Bomba    |
  +------v------+        +-------------+
  |   Sensor    | <-------------------------------------------------+
  +-------------+                                                   |
         ^                                                          |
         | (Herança)                                                | Usa
  +------+-------+---------------+-----------------+          +-----+---------------+
  |              |               |                 |          |    SensorFactory    |
SensorNivel  SensorVazao   sensorPressao   sensorContaminacao +---------------------+
```
* EstacaoBombeamento: Agregador principal que gerencia o ciclo (processarCiclo()) e coordena os componentes.
* Tanque: Encapsula o estado físico do fluido e executa as equações dinâmicas.
* Bomba: Representa os atuadores físicos (propriedades de estado, vazão, queima/falha).
* Sensor (Abstrata): Classe base para a telemetria, estendida por SensorNivel, SensorPressao, SensorVazao  e SensorContaminacao.
* SensorFactory: Implementação do padrão de projeto criacional para encapsular a instanciação dos sensores.
---

## Instruções de Compilação e Execução

 Pré-requisitosCompilador G++ com suporte a C++17 instalado.
 Python 3.8 ou superior com pip configurado.
 1. Compilar o Back-End (C++)No terminal, dentro do diretório do projeto, execute o comando de compilação unificado contendo todas as unidades de tradução:Bashg++ -std=c++17 main.cpp EstacaoBombeamento.cpp Tanque.cpp SensorFactory.cpp Bomba.cpp Sensor.cpp SensorNivel.cpp SensorPressao.cpp SensorVazao.cpp      SensorContaminacao.cpp -o simulador
   Para rodar a simulação via terminal:./simulador
   2. Executar o Front-End (Streamlit Python)Primeiro, instale as dependências da interface gráfica:Bashpip install streamlit pandas matplotlib
   Em seguida, inicialize o servidor do dashboard local:Bashstreamlit run app.py
   3. Para validar a consistência física e lógica do simulador de forma automatizada, foi implementada uma bateria de testes simplificada. Os testes cobrem:A não-estouro da capacidade do tanque (std::clamp).Instanciação correta via SensorFactory.

## Decisões de Padrões de Projeto

1. Padrão de Projeto: Factory (Fábrica de Sensores) Para o gerenciamento e inicialização dos dispositivos de telemetria, foi implementado o padrão SensorFactory.
Por que essa escolha? A estação inicializa sua rotina lendo um arquivo de configuração estática. O padrão Factory permite o desacoplamento total entre a classe principal (EstacaoBombeamento) e as implementações específicas de hardware (sensores de Nível, Vazão, Pressão, Contaminação).
Benefício: Facilidade de expansão. Adicionar um novo tipo de sensor ao sistema exige apenas atualizar a fábrica, sem modificar a lógica central do ciclo de bombeamento.
2. Gerenciamento de Memória: Passagem por Referência de Ponteiros (std::vector<Bomba*>)Decisão de arquitetura de memória tomada para evitar o custo de alocação de cópias sob a CPU e garantir que as alterações físicas de queima ou desarme de atuadores persistam de forma unificada tanto no escopo da Estação quanto no simulador matemático do Tanque.

3. Estratégia de Persistência de Dados: CSV em vez de SQL (MySQL)Para o armazenamento do histórico de telemetria e eventos da planta, optou-se pela geração de arquivos CSV (Comma-Separated Values) em detrimento de um Banco de Dados Relacional (SQL).
    1. Zero Infraestrutura: O sistema dispensa dependências complexas de rede, contêineres Docker ou instalação de servidores locais de banco de dados, rodando através da biblioteca nativa "fstream".
    2. Eficiência de Memória: O método de persistência grava os dados diretamente no disco via append (std::ios::app) ao final de cada ciclo. Isso elimina o acúmulo de logs na memória RAM, mitigando o risco de falhas por Out of Memory em regimes de operação contínua (24/7).

## Limitações Conhecidas

1. Simulação Monothread: O loop físico do tanque roda sob o mesmo fio de execução (thread) que a escrita de logs em disco, o que pode causar micro-atrasos perceptíveis se o intervalo de ciclo for reduzido para menos de 100ms.
2. Acoplamento por Arquivo Local (I/O Síncrono): A troca de dados entre C++ e Python ocorre estritamente via leitura e escrita síncrona no arquivo historico_estacao.csv. O front-end precisa ler o arquivo do disco a cada ciclo para atualizar os gráficos.

## *Assinatura Operacional da Dupla: 0016*
Projeto homologado e pronto para produção.
 Mateus Medeiros / Julia Roveta
