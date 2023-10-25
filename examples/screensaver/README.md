# Integrantes

- Gabriel Sponda Freitas Bettarello - 11201932580
- João Vitor Arruda Bartolo - 11201812168

---

# Projeto Tela de Descanso

O projeto visa desenvolver uma tela de descanso inspirada na do DVD, onde um logo se move e rebate nas bordas da tela. No entanto, em nossa implementação, o logo foi substituído por um círculo. Este círculo permite ajustes no raio, na velocidade de deslocamento e na ativação/desativação de um rastro, cuja intensidade pode ser modificada. Utilizamos o "Colored Triangles" como base para este projeto.

## main.cpp
Realizamos modificações sutis, incluindo a definição de “.vSync = 1” e a nomeação do projeto.

## window.hpp
Preservamos as funções originais, mas introduzimos novas variáveis:
- `m_center`: Define o centro do círculo.
- `m_speed`: Associado à taxa de atualização da tela e ao rastro do círculo.
- `m_dx`, `m_dy` e `m_magnitude`: Vetores que controlam o deslocamento do círculo. Nota-se que `m_dx` e `m_dy` são inicializados aleatoriamente, enquanto `m_magnitude` representa a norma do vetor resultante.
- `m_radius`: Representa o raio do círculo.
- `m_allowTrail` e `m_trail`: A primeira controla a ativação do rastro e a segunda sua intensidade.
- `m_positions`: Estabelece pontos para desenhar o círculo e um quadrilátero na tela.
- `m_colors`: Atribui cores a esses pontos.

## window.cpp
- `onCreate()`: Mantivemos a estrutura principal, adicionando suporte à transparência e definindo as posições dos cantos da janela. Além disso, normalizamos as variáveis m_dx e m_dy.
- `onPaintUI()`: Excluímos campos de edição de cores e incorporamos controles para o círculo e seu rastro. Esses controles são feitos a partir de sliders e uma check box, sendo que um dos sliders só é apresentado caso a check box esteja marcada.
- `onPaint()`: A função original de desenho foi adaptada para verificar o estado do rastro (m_allowTrail) e, se desativado, limpar o buffer. Adicionamos funções para desenhar o quadrilátero e o círculo, repare que o quadrilatero é desenhado sobre toda a janela e após isso o círculo é desenhado em sua posição correta, então as últimas quatro posições de m_positions estão relacionadas com o quadrilatero. Também incluímos um trecho de código para evitar a distorção das formas em tela cheia.
- `setupModel()`: Esta seção teve as maiores alterações. Mantivemos a estrutura de VBOs e VAO, mas aprimoramos a lógica para desenhar o círculo corretamente. Primeiramente, definimos as variáveis `angle` e `angleIncrement`, que são empregadas no subsequente laço `for`. Dentro deste laço, com base na posição atual do centro do círculo (`m_center`), seu raio (`m_radius`) e as variáveis angulares, determinamos 16 pontos circundantes ao centro. Estes pontos são armazenados no vetor `m_positions`. A cada iteração, o ângulo é incrementado, permitindo a correta geração dos pontos. Se desejássemos mais pontos, ajustaríamos o `angleIncrement`, reduzindo seu intervalo. Posteriormente, estabelecemos os valores de `m_colors` para as posições de 16 a 19, correspondentes às cores do quadrilátero que preenche a tela. Embora a cor base seja preta, modificamos apenas o parâmetro de opacidade. Esta opacidade é influenciada por `m_trail` e pela velocidade do círculo: uma maior velocidade resulta em menor transparência, tornando o rastro mais visível por mais tempo. Analogamente, ao elevar `m_trail`, a opacidade reduz. Por isso, o slider foi configurado de modo que, ao deslocar à esquerda, o rastro diminui, e à direita, ele aumenta. Nesta etapa, também atualizamos os valores centrais, incrementando os componentes x e y com `m_dx` e `m_dy`. Se a soma da posição central e do raio exceder as bordas da janela, `m_dx` ou `m_dy` invertem o sinal, fazendo com que o círculo rebata nas bordas.

**Observação**: Com `m_trail` em 0, o quadrilátero torna-se completamente transparente. Se o rastro estiver ativo, o buffer acumula quadros, criando uma trilha branca que preenche a tela. A sobreposição de imagens transparentes gera uma cor mais opaca, proporcionando a ilusão de um rastro.
