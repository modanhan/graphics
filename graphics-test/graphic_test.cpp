#include "stdafx.h"
#include "CppUnitTest.h"

#include "../graphics/GraphicProgram.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace graphicstest {
	TEST_CLASS(GraphicTest) {
public:
	TEST_METHOD(TestMethod1)
	{
		auto shader = Shader::Create(GL_COMPUTE_SHADER, "shaders/compute.glsl");
	}
	};
}