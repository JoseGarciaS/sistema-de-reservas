#include "app.h"
#include "imgui.h"
#include "templateWindow.h"

namespace app
{
	void RenderWindows()
	{
		ImGui::ShowDemoWindow();
		templateWindow::Render();
	}
}