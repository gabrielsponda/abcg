# Integrantes

- Gabriel Sponda Freitas Bettarello - 11201932580

- João Vitor Arruda Bartolo - 11201812168

  

---



# Lighting

Este projeto é uma evolução do "viewer6" e visa destacar as diferenças entre dois tipos de iluminação em um ambiente 3D: direcional e pontual. O objetivo é demonstrar como cada tipo de iluminação influencia a visualização de múltiplos objetos em uma cena, enriquecendo a compreensão sobre os efeitos e comportamentos de diferentes fontes de luz.

## Características

- **Modelos Multiplicados**: O modelo original do coelho foi multiplicado por 10, criando uma cena com vários coelhos.
- **Movimento e Rotação**: Cada coelho na cena pode se mover livremente pela tela e rotacionar em torno do próprio eixo.
- **Controle de Velocidade e Escala**: A velocidade de rotação e de deslocamento dos coelhos pela tela é ajustável. Além disso, é possível alterar a escala dos modelos.
- **Iluminação Direcional e Pontual**: O projeto inclui uma fonte de iluminação direcional, simulando a luz do sol, e uma fonte de iluminação pontual, proporcionando diferentes efeitos de iluminação nos modelos.

## Alterações no Código

As principais alterações no código incluem:

### `Window.hpp`

1. **Propriedades de Luz**:

   ```
   glm::vec4 m_lightPos{-1.0f, -1.0f, -1.0f, 1.0f};
   ```

   Aqui, `m_lightPos` é uma nova propriedade adicionada para definir a posição da fonte de luz pontual na cena. O valor `{1.0f}` na quarta componente indica que se trata de uma fonte de luz pontual (em contraste com a luz direcional, onde esta componente seria `0.0f`).

2. **Estrutura `Bunny` e Propriedades Relacionadas**:

   ```
   std::default_random_engine m_randomEngine;
   
   struct Bunny {
     glm::vec3 m_position{};
     glm::vec3 m_rotationAxis{};
     glm::vec3 m_direction{};
   };
   
   std::array<Bunny, 10> m_bunnies;
   
   float m_scale{0.1f};
   float m_movementSpeed{0.5f};
   float m_rotationSpeed{0.5f};
   float m_angle{};
   
   void randomizeBunny(Bunny &bunny);
   ```

   Este trecho é fundamental para a interatividade e dinâmica do projeto. A estrutura `Bunny` é definida para representar cada coelho na cena, contendo informações sobre posição, eixo de rotação e direção de movimento. A seguir, algumas propriedades e métodos importantes são introduzidos:

   - `m_randomEngine`: Motor para geração de números aleatórios.
   - `m_bunnies`: Um array de coelhos, indicando que vários modelos de coelhos são usados na cena.
   - `m_scale`, `m_movementSpeed`, `m_rotationSpeed`, `m_angle`: Propriedades para controlar a escala, a velocidade de movimento, a velocidade de rotação e o ângulo de rotação dos coelhos, respectivamente.
   - `randomizeBunny`: Método para inicializar cada coelho com valores aleatórios, contribuindo para a diversidade e dinamismo visual na cena.

### `Window.cpp`

1. **Inicialização e Movimentação dos Coelhos**:
   Inicialização dos coelhos com posições, eixos de rotação e direções iniciais aleatórias.

   ```c++
   void Window::onCreate() {
     /* Outras partes do código */
       
     for (auto &bunny : m_bunnies) {
       randomizeBunny(bunny);
     }
   }
   ```

   Neste trecho, todos os coelhos são inicializados no momento da criação da janela. O método `randomizeBunny` é chamado para cada coelho no array `m_bunnies`, definindo suas posições, eixos de rotação e direções iniciais aleatórias.

2. **Alterações na Iluminação e Renderização dos Coelhos**:
   Introdução de uma nova localização uniforme para a posição da luz pontual e renderização de cada coelho na cena.

   ```c++
   void Window::onPaint() {
     /* Outras partes do código */
       
     auto const lightPosLoc =
         abcg::glGetUniformLocation(program, "lightPosWorldSpace");
     
     /* Outras partes do código */
     
     auto const lightPosRotated{m_trackBallLight.getRotation() * m_lightPos};
     abcg::glUniform4fv(lightPosLoc, 1, &lightPosRotated.x);
     
     /* Outras partes do código */
     
     // Set uniform variables for the current model
     abcg::glUniform4fv(KaLoc, 1, &m_Ka.x);
     abcg::glUniform4fv(KdLoc, 1, &m_Kd.x);
     abcg::glUniform4fv(KsLoc, 1, &m_Ks.x);
     abcg::glUniform1f(shininessLoc, m_shininess);
   
     for (auto &bunny : m_bunnies) {
       glm::mat4 modelMatrix{1.0f};
       modelMatrix = glm::translate(modelMatrix, bunny.m_position);
       modelMatrix = glm::scale(modelMatrix, glm::vec3(m_scale));
       modelMatrix = glm::rotate(modelMatrix, m_angle, bunny.m_rotationAxis);
   
       abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);
   
       auto const modelViewMatrix{glm::mat3(m_viewMatrix * modelMatrix)};
       auto const normalMatrix{glm::inverseTranspose(modelViewMatrix)};
       abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);
   
       auto const lightDirRotated{m_trackBallLight.getRotation() * m_lightDir};
       abcg::glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
   
       auto const lightPosRotated{m_trackBallLight.getRotation() * m_lightPos};
       abcg::glUniform4fv(lightPosLoc, 1, &lightPosRotated.x);
   
       m_model.render(m_trianglesToDraw);
     }
   }
   ```

   - **Localização da Luz no Shader**: A posição da luz pontual no espaço mundial é obtida do shader. Isso é essencial para informar ao shader onde a luz está localizada no mundo 3D.
   - **Rotação da Posição da Luz**: A posição da luz é ajustada pela rotação da trackball, o que permite que a luz se mova conforme a interação do usuário. A nova posição rotacionada é então passada ao shader.
   - **Configuração das Propriedades do Material**: As propriedades do material (cor ambiente, difusa, especular e brilho) são configuradas no shader. Estes parâmetros determinam como a superfície do modelo reage à iluminação.
   - **Renderização dos Coelhos**:
     - Para cada coelho, uma matriz de modelo é criada para posicionar, escalar e rotacionar o coelho na cena.
     - Essa matriz de modelo é enviada para o shader para posicionar adequadamente cada coelho.
     - As matrizes de vista-modelo e normal são calculadas e enviadas ao shader, sendo utilizadas para transformações de iluminação e cálculos de sombreamento.
     - A direção da luz e a posição da luz (já ajustadas anteriormente) são passadas ao shader.
     - Por fim, o modelo de cada coelho é renderizado na cena.

3. **Atualização de Posição e Rotação dos Coelhos**:
   Atualização do ângulo de rotação e da posição de cada coelho, permitindo movimentação dinâmica na cena.

   ```c++
   void Window::onUpdate() {
     /* Outras partes do código */
   	    
     auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
     m_angle += glm::radians(90.0f * m_rotationSpeed) * deltaTime;
   
     for (auto &bunny : m_bunnies) {
       // Calcule a nova posição com base na direção e na velocidade dos coelhos
       bunny.m_position += bunny.m_direction * m_movementSpeed * deltaTime;
   
       // Se um coelho sair da tela, realoque-o com uma nova posição inicial
       if (glm::length(bunny.m_position.x) > 3.0f ||
           glm::length(bunny.m_position.x) < -3.0f ||
           glm::length(bunny.m_position.y) > 3.0f ||
           glm::length(bunny.m_position.y) < -3.0f) {
         randomizeBunny(bunny);
       }
     }
   }
   ```

   Aqui, o ângulo de rotação de cada coelho é atualizado com base em um valor de velocidade (`m_rotationSpeed`). Além disso, a posição de cada coelho é atualizada, permitindo que eles se movam pela tela.

4. **Controles de Interface do Usuário para Escala e Velocidade**:
   Adição de controles deslizantes para ajustar a escala, velocidade de movimento e velocidade de rotação dos coelhos.

   ```c++
   void Window::onPaintUI() {
     /* Outras partes do código */
       
     ImGui::SliderFloat("Scale", &m_scale, 0.1f, 1.0f, "Escala: %.1f");
     ImGui::SliderFloat("rotationSpeed", &m_movementSpeed, 0.0f, 1.0f,
                        "Velocidade de movimento: %.1f");
     ImGui::SliderFloat("movementSpeed", &m_rotationSpeed, 0.0f, 1.0f,
                        "Velocidade de rotação: %.1f");
       
     /* Outras partes do código */
   }
   ```

   Esta parte adiciona controles deslizantes na interface do usuário para ajustar a escala dos coelhos (`m_scale`), a velocidade de movimento (`m_movementSpeed`) e a velocidade de rotação (`m_rotationSpeed`).

5. **Função para Randomizar Posições e Direções dos Coelhos**:
   Função para atribuir valores aleatórios para a posição inicial, eixo de rotação e direção de movimento de cada coelho.

   ```C++
   void Window::randomizeBunny(Bunny &bunny) {
     std::uniform_real_distribution<float> xDistribution(
         -3.0f, 3.0f); // Limites ajustados
     std::uniform_real_distribution<float> yDistribution(
         -3.0f, 3.0f); // Limites ajustados
     std::uniform_real_distribution<float> zDistribution(
         -3.0f, -3.0f); // Limites ajustados para o eixo Z
   
     // Gerar posições iniciais fora da tela
     bunny.m_position =
         glm::vec3(xDistribution(m_randomEngine), yDistribution(m_randomEngine),
                   zDistribution(m_randomEngine));
   
     // Certificar-se de que os coelhos não apareçam no campo de visão
     while (glm::length(bunny.m_position) < 1.0f) {
       bunny.m_position = glm::vec3(
           xDistribution(m_randomEngine) / yDistribution(m_randomEngine),
           yDistribution(m_randomEngine) / zDistribution(m_randomEngine),
           zDistribution(m_randomEngine) / xDistribution(m_randomEngine));
     }
   
     // Gerar um eixo de rotação aleatório
     bunny.m_rotationAxis = glm::normalize(
         glm::vec3(xDistribution(m_randomEngine), yDistribution(m_randomEngine),
                   zDistribution(m_randomEngine)));
   
     // Permitir que os coelhos se movam ao longo dos eixos X, Y e Z
     std::uniform_real_distribution<float> directionDistribution(-1.0f, 1.0f);
     bunny.m_direction = glm::vec3(directionDistribution(m_randomEngine),
                                   directionDistribution(m_randomEngine),
                                   directionDistribution(m_randomEngine));
   
     // Certificar-se de que os coelhos não fiquem parados
     while (bunny.m_direction.x == 0 && bunny.m_direction.x == 0) {
       bunny.m_direction = glm::vec3(directionDistribution(m_randomEngine),
                                     directionDistribution(m_randomEngine),
                                     directionDistribution(m_randomEngine));
     }
   }
   ```

   - **Inicialização da Posição**: A função começa criando três distribuições de números aleatórios para os eixos X, Y e Z. Essas distribuições são usadas para gerar uma posição inicial aleatória para o coelho, garantindo que ele comece fora do campo de visão do usuário.
   - **Verificação da Posição Inicial**: Um loop `while` garante que o coelho não apareça muito perto do centro da cena (campo de visão imediato), realocando-o se necessário.
   - **Geração do Eixo de Rotação**: Um eixo de rotação aleatório é gerado para cada coelho, usando as mesmas distribuições de números aleatórios. Isso determina como o coelho irá girar sobre si mesmo.
   - **Definição da Direção de Movimento**: A direção na qual cada coelho se move é definida de forma aleatória. Um segundo loop `while` garante que os coelhos sempre tenham alguma direção de movimento, evitando que fiquem parados.

### `blinnphongponctual.vert`

- **Declaração da Posição da Luz**: Aqui, uma variável uniforme chamada `lightPosWorldSpace` é declarada para armazenar a posição da luz pontual no espaço do mundo. Essa variável será usada para calcular a direção da luz em relação à posição dos vértices do modelo.

  ````vert
  vec4 lightDirWorldSpace = lightPosWorldSpace - (modelMatrix * vec4(inPosition, 1.0));
  ````
