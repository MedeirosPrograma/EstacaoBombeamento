
# mateus medeiros

## 1.analise o arquivo Sensor.h para possiveis erros

Aqui estão os problemas encontrados e como corrigi-los:

 O Destrutor Não é Virtual (Grave)
Como a classe Sensor possui métodos virtuais puros (= 0), ela foi feita para ser uma classe base abstrata (interface). Como vimos antes, se você deletar um objeto de uma classe filha usando um ponteiro do tipo Sensor*, o destrutor da classe filha não será chamado, gerando memory leak.
Como corrigir: Altere para virtual ~Sensor() = default;.

 Uso de using namespace std; em Arquivos de Cabeçalho (.h) (Prática Perigosa)
Colocar using namespace std; dentro de um arquivo .h faz com que todo arquivo que inclua Sensor.h também importe o namespace std inteiro automaticamente. Isso quebra o encapsulamento de nomes e pode causar conflitos de assinaturas (ex: se você criar uma classe chamada Distance, ela pode conflitar com std::distance).
Como corrigir: Remova o using namespace std; e use o prefixo std:: explicitamente para std::string.

 Passagem de string por valor no Construtor
No construtor Sensor(string tag, string unidade);, os parâmetros estão sendo passados por valor. Isso gera cópias desnecessárias da string na memória.
Como corrigir: Passe por referência constante (const std::string&).

* *todas as 3 alterações foram levadas em consideração para o arquivo em questão e tambem para arquivos futuros*

=======

# julia roveta