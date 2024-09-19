#include "app.h"
#include "imgui.h"
#include "templateWindow.h"
//#include "pingTest.h"

using namespace std;

namespace app
{
	void RenderWindows()
	{
		ImGui::ShowDemoWindow();
		//pingTest::Render();
	}

}