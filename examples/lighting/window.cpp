#include "window.hpp"

#include "imfilebrowser.h"

void Window::onEvent(SDL_Event const &event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_MOUSEMOTION) {
    m_trackBallModel.mouseMove(mousePosition);
    m_trackBallLight.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackBallModel.mousePress(mousePosition);
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      m_trackBallLight.mousePress(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackBallModel.mouseRelease(mousePosition);
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
      m_trackBallLight.mouseRelease(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEWHEEL) {
    /*----------*/
    // m_lightPos.z += 0.1f * event.wheel.y;
    /*----------*/
  }
}

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  // Create programs
  for (auto const &name : m_shaderNames) {
    auto const path{assetsPath + "shaders/" + name};
    auto const program{abcg::createOpenGLProgram(
        {{.source = path + ".vert", .stage = abcg::ShaderStage::Vertex},
         {.source = path + ".frag", .stage = abcg::ShaderStage::Fragment}})};
    m_programs.push_back(program);
  }

  // Load default model
  loadModel(assetsPath + "bunny.obj");

  // Initial trackball spin
  m_trackBallModel.setAxis(glm::normalize(glm::vec3(1, 1, 1)));
  m_trackBallModel.setVelocity(0.1f);

  createSkybox();

  /*----------*/
  for (auto &bunny : m_bunnies) {
    randomizeBunny(bunny);
  }
  /*----------*/
}

void Window::loadModel(std::string_view path) {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  m_model.destroy();

  m_model.loadDiffuseTexture(assetsPath + "maps/pattern.png");
  m_model.loadNormalTexture(assetsPath + "maps/pattern_normal.png");
  m_model.loadCubeTexture(assetsPath + "maps/cube/");
  m_model.loadObj(path);
  m_model.setupVAO(m_programs.at(m_currentProgramIndex));
  m_trianglesToDraw = m_model.getNumTriangles();

  // Use material properties from the loaded model
  m_Ka = m_model.getKa();
  m_Kd = m_model.getKd();
  m_Ks = m_model.getKs();
  m_shininess = m_model.getShininess();
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  // Use currently selected program
  auto const program{m_programs.at(m_currentProgramIndex)};
  abcg::glUseProgram(program);

  // Get location of uniform variables
  auto const viewMatrixLoc{abcg::glGetUniformLocation(program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(program, "projMatrix")};
  auto const modelMatrixLoc{abcg::glGetUniformLocation(program, "modelMatrix")};
  auto const normalMatrixLoc{
      abcg::glGetUniformLocation(program, "normalMatrix")};
  auto const lightDirLoc{
      abcg::glGetUniformLocation(program, "lightDirWorldSpace")};

  /*----------*/
  auto const lightPosLoc =
      abcg::glGetUniformLocation(program, "lightPosWorldSpace");
  /*----------*/

  auto const shininessLoc{abcg::glGetUniformLocation(program, "shininess")};
  auto const IaLoc{abcg::glGetUniformLocation(program, "Ia")};
  auto const IdLoc{abcg::glGetUniformLocation(program, "Id")};
  auto const IsLoc{abcg::glGetUniformLocation(program, "Is")};
  auto const KaLoc{abcg::glGetUniformLocation(program, "Ka")};
  auto const KdLoc{abcg::glGetUniformLocation(program, "Kd")};
  auto const KsLoc{abcg::glGetUniformLocation(program, "Ks")};
  auto const diffuseTexLoc{abcg::glGetUniformLocation(program, "diffuseTex")};
  auto const normalTexLoc{abcg::glGetUniformLocation(program, "normalTex")};
  auto const cubeTexLoc{abcg::glGetUniformLocation(program, "cubeTex")};
  auto const mappingModeLoc{abcg::glGetUniformLocation(program, "mappingMode")};
  auto const texMatrixLoc{abcg::glGetUniformLocation(program, "texMatrix")};

  // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform1i(diffuseTexLoc, 0);
  abcg::glUniform1i(normalTexLoc, 1);
  abcg::glUniform1i(cubeTexLoc, 2);
  abcg::glUniform1i(mappingModeLoc, m_mappingMode);

  glm::mat3 const texMatrix{m_trackBallLight.getRotation()};
  abcg::glUniformMatrix3fv(texMatrixLoc, 1, GL_TRUE, &texMatrix[0][0]);

  auto const lightDirRotated{m_trackBallLight.getRotation() * m_lightDir};
  abcg::glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);

  /*----------*/
  auto const lightPosRotated{m_trackBallLight.getRotation() * m_lightPos};
  abcg::glUniform4fv(lightPosLoc, 1, &lightPosRotated.x);
  /*----------*/

  abcg::glUniform4fv(IaLoc, 1, &m_Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &m_Id.x);
  abcg::glUniform4fv(IsLoc, 1, &m_Is.x);

  /*----------*/
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
  /*----------*/

  abcg::glUseProgram(0);

  if (m_currentProgramIndex == 0 || m_currentProgramIndex == 1) {
    renderSkybox();
  }
}

void Window::onUpdate() {
  m_modelMatrix = m_trackBallModel.getRotation();

  m_viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f + m_zoom),
                  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  /*----------*/
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
  /*----------*/
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  auto const scaledWidth{gsl::narrow_cast<int>(m_viewportSize.x * 0.8f)};
  auto const scaledHeight{gsl::narrow_cast<int>(m_viewportSize.y * 0.8f)};

  // File browser for models
  static ImGui::FileBrowser fileDialogModel;
  fileDialogModel.SetTitle("Load 3D Model");
  fileDialogModel.SetTypeFilters({".obj"});
  fileDialogModel.SetWindowSize(scaledWidth, scaledHeight);

  // File browser for textures
  static ImGui::FileBrowser fileDialogDiffuseMap;
  fileDialogDiffuseMap.SetTitle("Load Diffuse Map");
  fileDialogDiffuseMap.SetTypeFilters({".jpg", ".png"});
  fileDialogDiffuseMap.SetWindowSize(scaledWidth, scaledHeight);

  // File browser for normal maps
  static ImGui::FileBrowser fileDialogNormalMap;
  fileDialogNormalMap.SetTitle("Load Normal Map");
  fileDialogNormalMap.SetTypeFilters({".jpg", ".png"});
  fileDialogNormalMap.SetWindowSize(scaledWidth, scaledHeight);

#if defined(__EMSCRIPTEN__)
  auto const assetsPath{abcg::Application::getAssetsPath()};
  fileDialogModel.SetPwd(assetsPath);
  fileDialogDiffuseMap.SetPwd(assetsPath + "/maps");
  fileDialogNormalMap.SetPwd(assetsPath + "/maps");
#endif

  // Create main window widget
  {
    auto widgetSize{ImVec2(222, 190)};

    if (!m_model.isUVMapped()) {
      // Add extra space for static text
      widgetSize.y += 26;
    }

    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr,
                 ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration);

    // Menu
    {
      bool loadModel{};
      bool loadDiffuseMap{};
      bool loadNormalMap{};
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          ImGui::MenuItem("Load 3D Model...", nullptr, &loadModel);
          ImGui::MenuItem("Load Diffuse Map...", nullptr, &loadDiffuseMap);
          ImGui::MenuItem("Load Normal Map...", nullptr, &loadNormalMap);
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
      if (loadModel)
        fileDialogModel.Open();
      if (loadDiffuseMap)
        fileDialogDiffuseMap.Open();
      if (loadNormalMap)
        fileDialogNormalMap.Open();
    }

    // Slider will be stretched horizontally
    ImGui::PushItemWidth(widgetSize.x - 16);
    ImGui::SliderInt(" ", &m_trianglesToDraw, 0, m_model.getNumTriangles(),
                     "%d triangles");
    /*----------*/
    ImGui::SliderFloat("Scale", &m_scale, 0.1f, 1.0f, "Escala: %.1f");
    ImGui::SliderFloat("rotationSpeed", &m_movementSpeed, 0.0f, 1.0f,
                       "Velocidade de movimento: %.1f");
    ImGui::SliderFloat("movementSpeed", &m_rotationSpeed, 0.0f, 1.0f,
                       "Velocidade de rotação: %.1f");
    /*----------*/
    ImGui::PopItemWidth();

    static bool faceCulling{};
    ImGui::Checkbox("Back-face culling", &faceCulling);

    if (faceCulling) {
      abcg::glEnable(GL_CULL_FACE);
    } else {
      abcg::glDisable(GL_CULL_FACE);
    }

    // CW/CCW combo box
    {
      static std::size_t currentIndex{};
      std::vector<std::string> const comboItems{"CCW", "CW"};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Front face",
                            comboItems.at(currentIndex).c_str())) {
        for (auto const index : iter::range(comboItems.size())) {
          auto const isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      if (currentIndex == 0) {
        abcg::glFrontFace(GL_CCW);
      } else {
        abcg::glFrontFace(GL_CW);
      }
    }

    // Projection combo box
    {
      static std::size_t currentIndex{};
      std::vector<std::string> comboItems{"Perspective", "Orthographic"};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Projection",
                            comboItems.at(currentIndex).c_str())) {
        for (auto const index : iter::range(comboItems.size())) {
          auto const isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                        gsl::narrow<float>(m_viewportSize.y)};
      if (currentIndex == 0) {
        m_projMatrix =
            glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);
      } else {
        m_projMatrix =
            glm::ortho(-1.0f * aspect, 1.0f * aspect, -1.0f, 1.0f, 0.1f, 5.0f);
      }
    }

    // Shader combo box
    {
      static std::size_t currentIndex{};

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("Shader", m_shaderNames.at(currentIndex))) {
        for (auto const index : iter::range(m_shaderNames.size())) {
          auto const isSelected{currentIndex == index};
          if (ImGui::Selectable(m_shaderNames.at(index), isSelected))
            currentIndex = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      // Set up VAO if shader program has changed
      if (gsl::narrow<int>(currentIndex) != m_currentProgramIndex) {
        m_currentProgramIndex = gsl::narrow<int>(currentIndex);
        m_model.setupVAO(m_programs.at(m_currentProgramIndex));
      }
    }

    if (!m_model.isUVMapped()) {
      ImGui::TextColored(ImVec4(1, 1, 0, 1), "Mesh has no UV coords.");
    }

    // UV mapping box
    {
      std::vector<std::string> comboItems{"Triplanar", "Cylindrical",
                                          "Spherical"};

      if (m_model.isUVMapped())
        comboItems.emplace_back("From mesh");

      ImGui::PushItemWidth(120);
      if (ImGui::BeginCombo("UV mapping",
                            comboItems.at(m_mappingMode).c_str())) {
        for (auto const index : iter::range(comboItems.size())) {
          auto const isSelected{m_mappingMode == static_cast<int>(index)};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            m_mappingMode = index;
          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();
    }

    ImGui::End();
  }

  // Create window for light sources
  if (m_currentProgramIndex >= 2 && m_currentProgramIndex <= 6) {
    auto const widgetSize{ImVec2(222, 244)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5,
                                   m_viewportSize.y - widgetSize.y - 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoDecoration);

    ImGui::Text("Light properties");

    // Slider to control light properties
    ImGui::PushItemWidth(widgetSize.x - 36);
    ImGui::ColorEdit3("Ia", &m_Ia.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Id", &m_Id.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Is", &m_Is.x, ImGuiColorEditFlags_Float);
    ImGui::PopItemWidth();

    ImGui::Spacing();

    ImGui::Text("Material properties");

    // Slider to control material properties
    ImGui::PushItemWidth(widgetSize.x - 36);
    ImGui::ColorEdit3("Ka", &m_Ka.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Kd", &m_Kd.x, ImGuiColorEditFlags_Float);
    ImGui::ColorEdit3("Ks", &m_Ks.x, ImGuiColorEditFlags_Float);
    ImGui::PopItemWidth();

    // Slider to control the specular shininess
    ImGui::PushItemWidth(widgetSize.x - 16);
    ImGui::SliderFloat(" ", &m_shininess, 0.0f, 500.0f, "shininess: %.1f");
    ImGui::PopItemWidth();

    ImGui::End();
  }

  fileDialogModel.Display();
  if (fileDialogModel.HasSelected()) {
    loadModel(fileDialogModel.GetSelected().string());
    fileDialogModel.ClearSelected();

    if (m_model.isUVMapped()) {
      // Use mesh texture coordinates if available...
      m_mappingMode = 3;
    } else {
      // ...or triplanar mapping otherwise
      m_mappingMode = 0;
    }
  }

  fileDialogDiffuseMap.Display();
  if (fileDialogDiffuseMap.HasSelected()) {
    m_model.loadDiffuseTexture(fileDialogDiffuseMap.GetSelected().string());
    fileDialogDiffuseMap.ClearSelected();
  }

  fileDialogNormalMap.Display();
  if (fileDialogNormalMap.HasSelected()) {
    m_model.loadNormalTexture(fileDialogNormalMap.GetSelected().string());
    fileDialogNormalMap.ClearSelected();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_trackBallModel.resizeViewport(size);
  m_trackBallLight.resizeViewport(size);
}

void Window::onDestroy() {
  m_model.destroy();
  for (auto const &program : m_programs) {
    abcg::glDeleteProgram(program);
  }
}

void Window::createSkybox() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  // Create skybox program
  auto const path{assetsPath + "shaders/" + m_skyShaderName};
  m_skyProgram = abcg::createOpenGLProgram(
      {{.source = path + ".vert", .stage = abcg::ShaderStage::Vertex},
       {.source = path + ".frag", .stage = abcg::ShaderStage::Fragment}});

  // Generate VBO
  abcg::glGenBuffers(1, &m_skyVBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_skyVBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_skyPositions),
                     m_skyPositions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_skyProgram, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_skyVAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_skyVAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_skyVBO);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Window::renderSkybox() {
  abcg::glUseProgram(m_skyProgram);

  auto const viewMatrixLoc{
      abcg::glGetUniformLocation(m_skyProgram, "viewMatrix")};
  auto const projMatrixLoc{
      abcg::glGetUniformLocation(m_skyProgram, "projMatrix")};
  auto const skyTexLoc{abcg::glGetUniformLocation(m_skyProgram, "skyTex")};

  auto const viewMatrix{m_trackBallLight.getRotation()};
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform1i(skyTexLoc, 0);

  abcg::glBindVertexArray(m_skyVAO);

  abcg::glActiveTexture(GL_TEXTURE0);
  abcg::glBindTexture(GL_TEXTURE_CUBE_MAP, m_model.getCubeTexture());

  abcg::glEnable(GL_CULL_FACE);
  abcg::glFrontFace(GL_CW);
  abcg::glDepthFunc(GL_LEQUAL);
  abcg::glDrawArrays(GL_TRIANGLES, 0, m_skyPositions.size());
  abcg::glDepthFunc(GL_LESS);

  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void Window::destroySkybox() const {
  abcg::glDeleteProgram(m_skyProgram);
  abcg::glDeleteBuffers(1, &m_skyVBO);
  abcg::glDeleteVertexArrays(1, &m_skyVAO);
}

/*----------*/
// Função para gerar coelhos aleatórios
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
/*----------*/