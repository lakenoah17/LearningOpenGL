#include "TestBatchRendering.h"

#include <memory>

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


namespace test {

	TestBatchRendering::TestBatchRendering()
		: m_Translation(200, 200, 0),
		m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))
	{
		float positions[] = {
			-50.0f, -50.0f, 0.0f, 0.0f,
			 50.0f, -50.0f, 1.0f, 0.0f,
			 50.0f,  50.0f, 1.0f, 1.0f,
			-50.0f,  50.0f, 0.0f, 1.0f,

			  0.0f,   0.0f, 0.0f, 0.0f,
			100.0f,   0.0f, 1.0f, 0.0f,
			100.0f, 100.0f, 1.0f, 1.0f,
			  0.0f, 100.0f, 0.0f, 1.0f
		};

		unsigned int indicies[] = {
			0, 1, 2,
			2, 3, 0,

			4, 5, 6,
			6, 7, 4
		};

		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GLCall(glEnable(GL_BLEND));

		m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
		m_VAO = std::make_unique<VertexArray>();

		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 8 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(indicies, 12);

		m_Shader->Bind();


		m_Texture = std::make_unique<Texture>("res/textures/destroyer.png");
		m_Shader->SetUniform1i("u_Texture", 0);
	}

	TestBatchRendering::~TestBatchRendering()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f))
	}

	void TestBatchRendering::OnUpdate(float deltaTime)
	{
	}

	void TestBatchRendering::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer renderer;

		m_Texture->Bind();

		//What is entailed in a draw call
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}
	}

	void TestBatchRendering::OnImGuiRender()
	{
		ImGui::SliderFloat3("Translation: ", &m_Translation.x, 0.0f, 960.0f);
		ImGui::Text("Application avg %.3f", 1000.0f / ImGui::GetIO().Framerate);
	}
}