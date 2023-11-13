# Integrantes

- Gabriel Sponda Freitas Bettarello - 11201932580
- João Vitor Arruda Bartolo - 11201812168

---

# Drone Pilot

Este projeto é uma adaptação do projeto "LookAt" que oferece uma experiência de pilotagem de drone em primeira pessoa. Além disso, foram implementadas animações no chão para tornar a experiência mais dinâmica. O objetivo é proporcionar aos usuários a sensação de controlar um drone enquanto exploram um ambiente tridimensional envolvente. A seguir, serão detalhadas as modificações e implementações específicas deste projeto.

## COMANDOS

``W`` e ``S``: Movimento *dolly*

``A`` e ``D``: Movimento *truck*

``Q`` e ``E``: Movimento *roll*

``UP`` e ``DOWN``: Movimento *tilt*

``LEFT`` e ``RIGHT``: Movimento *pan*

``SPACE`` e ``LCTRL``: Movimento *pedestal*

``t``: Alterna entre cubos grande e pequenos (*togle*)

``0``: Shader *original*

``1``: Shader *rows*

``2``: Shader *columns*

``3``: Shader *sheet*

``4``: Shader *xRotation*

``5``: Shader *zRotation*

``6``: Shader *wave*

``7``: Shader *cubeWave*


## Camera

As principais alterações no código são as adições de funcionalidades que permitem a rotação da câmera em torno de seus eixos. Aqui está um resumo breve e claro das alterações:

1. **Roll** (Rotação em torno do Eixo Z): A função roll foi adicionada para permitir a rotação da câmera em torno do seu eixo Z local.

2. **Tilt** (Rotação em torno do Eixo X): A função tilt foi implementada para possibilitar a rotação da câmera em torno do seu eixo X local.

3. **Pedestal** (Movimento Vertical): A função pedestal foi incluída para permitir o movimento vertical da câmera, movendo-a para cima ou para baixo.

Essas adições expandem a funcionalidade da classe da câmera, proporcionando um controle mais completo sobre sua orientação e posição no espaço tridimensional. Agora, é possível ajustar a câmera além dos movimentos de dolly, truck e pan já existentes.



## Ground

Originalmente, no projeto LookAt, o chão consistia em quadrados, onde cada quadrado era definido por apenas 4 vértices e renderizado utilizando o método `GL_TRIANGLE_STRIP`. Entretanto, realizamos uma modificação significativa ao aumentar o número de vértices para 36, transformando esses quadrados em cubos, e passamos a renderizá-los utilizando o método `GL_TRIANGLES`.

Adicionalmente, implementamos uma alteração na função de criação do chão, onde introduzimos um novo parâmetro para representar o tamanho do cubo (``void Ground::create(GLuint program, float size)``). Esse tamanho é então aplicado à posição dos vértices, permitindo que as arestas do cubo se ajustem conforme necessário, seja para aumentar ou diminuir seu tamanho. Vamos abordar em detalhes como esse parâmetro pode ser modificado posteriormente.



## Window

As principais mudanças são o mapeamento de teclas e a atualização de shaders. Além dessas, algumas mudanças mais utis forma feitas. A cor de fundo foi trocada para um azul céu. Os coelhos foram removidos e adicionamos esferas em posições formando uma espécie de "arco", para que possamos executar um movimento de *tilt* em 360º e ter de fato a impressão de que está funcionando, sem elas, temos um grande deslocamento olhando somente o "céu" e fica um aspecto esquisito.

#### onEvent()

Modificamos a configuração das teclas de movimento e introduzimos novos atalhos para o ajuste da posição da câmera. Também atribuímos às teclas numéricas a função de alternar entre diferentes shaders. Cada número agora corresponde a um shader único, alterando a renderização do chão com uma variedade de efeitos visuais, cujos detalhes abordaremos adiante.

#### loadShader()

A função ``loadShader()`` carrega, compila e vincula os shaders de vértice e fragmento em um programa de shader. Aqui está o que cada comando faz:

``abcg::Application::getAssetsPath()``: Obtém o caminho para o diretório onde os arquivos de shader estão localizados.

``abcg::glDeleteProgram()``: Exclui o programa de shader existente para liberar recursos.

`` abcg::createOpenGLProgram()``: Cria um novo programa de shader carregando, compilando e vinculando shaders dos caminhos vertexShaderPath e fragmentShaderPath.

``abcg::glGetUniformLocation()``: Obtém as localizações das variáveis uniformes no programa de shader, como viewMatrix, projMatrix, modelMatrix, color e time, armazenando-as para uso posterior em transformações, coloração e animação.

---

# Shaders

###### original (0)

Esse shader é o padrão, como no projeto original e a única mudança foi a remoção da intensidade de cor de acordo com a distância, para melhor visualização. 

###### rows (1)

Nesse sahder, os cubos ficam se deslocando em linhas intercaladamente, dando a impressão de que  isso é feito infinitamente, quando na verdade, a cada dois cubos deslocados cada linhas é "resetada".

###### columns (2)

Como o shader *rows*, mas dessa vez em colunas.

###### sheet (3)

Esse é uma mistura do *rows* com o *columns*, a cada dois cubos deslocados, o movimento é alternado entre os dois shaders.

###### xRotation (4)

Aqui os cubos rodam em torno do eixo X original.

###### zRotation (5)

Aqui os cubos rodam em torno do eixo Z original.

###### wave (6)

Nesse shader o chão se comporta como um líquido, parecendo o mar, formando uma onda/marola.

###### cubeWave (7)

Por último, esse shader faz com que cada cubo pareça estar flutuando de maneira independente na água.



