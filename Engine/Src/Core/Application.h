#pragma once
#include "bepch.h"

#include "Core.h"
#include "Utils/Log.h"
#include "Window.h"
#include "Input.h"

namespace Bonfire
{
	class Application
	{
	public:
		void Run();
		void Init();

		const RefPtr<Window>& GetWindow() const { return m_Window; }

	public:
		virtual Window::WindowProperties SetupWindowProperties();

	public:
		virtual void OnInit() = 0;
		virtual void OnUpdate() = 0;

	public:
		static const Application* Get() { return s_Instance; }

	private:
		RefPtr<Window> m_Window;

		static Application* s_Instance;
	};
}