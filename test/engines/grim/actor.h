// Note that you may have to set up the engine variables to test GRIM/EMI
#include <cxxtest/TestSuite.h>
#include <iostream>

#include "base/commandLine.h"
#include "engines/grim/grim.h"
#include "engines/grim/actor.h"
#include "common/config-manager.h"

#ifdef WIN32
#include "backends/platform/sdl/win32/win32.h"
#else
#ifdef POSIX
#include "backends/platform/sdl/posix/posix.h"
#else
#error "Add more systems!"
#endif
#endif

// Allowed error
#define EP		(1E-4f)

class GrimActorTestSuite : public CxxTest::TestSuite {
public:
	// Setup the engine variables
	void test_ActorSetup() {
#ifdef WIN32
		g_system = new OSystem_Win32;
		((OSystem_Win32 *)g_system)->init();
		((OSystem_Win32 *)g_system)->initBackend();
#else
#ifdef POSIX
		g_system = new OSystem_POSIX;
		((OSystem_POSIX *)g_system)->init();
		((OSystem_POSIX *)g_system)->initBackend();
#else
		#error "Add more systems!"
#endif
#endif
		Base::registerDefaults();
		TS_ASSERT(g_system != nullptr);
	}

	// Test Constructor
	void test_Actor() {
		Grim::Actor *a = new Grim::Actor();

		TS_ASSERT(a != nullptr);
	}

	// Test setting position
	void test_ActorPosition() {
		Grim::Actor *a = new Grim::Actor();
		Math::Vector3d pos(1.0f, 2.0f, 3.0f);

		a->setPos(pos);
		pos = a->getPos();

		TS_ASSERT(pos.x() == 1.0f);
		TS_ASSERT(pos.y() == 2.0f);
		TS_ASSERT(pos.z() == 3.0f);
	}

	// Test setting rotation
	void test_ActorRotation() {
		Grim::Actor *a = new Grim::Actor();
		Math::Angle pitch = 30.0f;
		Math::Angle yaw = 20.0f;
		Math::Angle roll = 10.0f;

		a->setRot(pitch, yaw, roll);

		TS_ASSERT(a->getPitch().getDegrees() == pitch.getDegrees());
		TS_ASSERT(a->getYaw().getDegrees() == yaw.getDegrees());
		TS_ASSERT(a->getRoll().getDegrees() == roll.getDegrees());
	}

	// Test Attaching for EMI
	void test_ActorAttach() {
		std::cout << std::endl;

		// We have to set up g_grim first, which needs g_system
		TS_ASSERT(g_system != nullptr);
		Grim::GrimEngine g = Grim::GrimEngine(g_system, 0, Grim::GType_MONKEY4, Common::kPlatformWindows, Common::EN_ANY);
		Grim::g_grim = &g;

		Grim::Actor *a1 = new Grim::Actor();
		Grim::Actor *a2 = new Grim::Actor();
		Grim::Actor *a3 = new Grim::Actor();

		// Set the position and rotation for the actors
		Math::Vector3d a1_pv(1.0f, 4.0f, 5.0f);
		a1->setPos(a1_pv);
		a1->setRot(5.0f, 15.0f, 25.0f);

		Math::Vector3d a2_pv(3.0f, -2.0f, -1.0f);
		a2->setPos(a2_pv);
		a2->setRot(15.0f, 25.0f, 35.0f);

		Math::Vector3d a3_pv(-3.0f, -4.0f, 2.0f);
		a3->setPos(a3_pv);
		a3->setRot(12.0f, 16.0f, 18.0f);

		// Attach a2 to a1
		a2->attachToActor(a1, nullptr);

		// Attach a3 to a2
		a3->attachToActor(a2, nullptr);

		// Check the world position (should be the same as before)
		Math::Vector3d a1_wpos = a1->getWorldPos();
		TS_ASSERT(fabs(a1_wpos.x() - 1.0f) < EP);
		TS_ASSERT(fabs(a1_wpos.y() - 4.0f) < EP);
		TS_ASSERT(fabs(a1_wpos.z() - 5.0f) < EP);
		std::cout << "a1 wpos: " << a1_wpos.x() << " " << a1_wpos.y() << " " << a1_wpos.z() << std::endl;

		Math::Vector3d a2_wpos = a2->getWorldPos();
		TS_ASSERT(fabs(a2_wpos.x() - 3.0f) < EP);
		TS_ASSERT(fabs(a2_wpos.y() - -2.0f) < EP);
		TS_ASSERT(fabs(a2_wpos.z() - -1.0f) < EP);
		std::cout << "a2 wpos: " << a2_wpos.x() << " " << a2_wpos.y() << " " << a2_wpos.z() << std::endl;

		Math::Vector3d a3_wpos = a3->getWorldPos();
		TS_ASSERT(fabs(a3_wpos.x() - -3.0f) < EP);
		TS_ASSERT(fabs(a3_wpos.y() - -4.0f) < EP);
		TS_ASSERT(fabs(a3_wpos.z() - 2.0f) < EP);
		std::cout << "a3 wpos: " << a3_wpos.x() << " " << a3_wpos.y() << " " << a3_wpos.z() << std::endl;

		// Check the relative position
		Math::Vector3d a1_pos = a1->getPos();
		TS_ASSERT(fabs(a1_pos.x() - 1.0f) < EP);
		TS_ASSERT(fabs(a1_pos.y() - 4.0f) < EP);
		TS_ASSERT(fabs(a1_pos.z() - 5.0f) < EP);
		std::cout << "a1 pos: " << a1_pos.x() << " " << a1_pos.y() << " " << a1_pos.z() << std::endl;

		Math::Vector3d a2_pos = a2->getPos();
		TS_ASSERT(fabs(a2_pos.x() - 0.854459f) < EP);
		TS_ASSERT(fabs(a2_pos.y() - -6.7806f) < EP);
		TS_ASSERT(fabs(a2_pos.z() - -5.41233f) < EP);
		std::cout << "a2 pos: " << a2_pos.x() << " " << a2_pos.y() << " " << a2_pos.z() << std::endl;

		Math::Vector3d a3_pos = a3->getPos();
		TS_ASSERT(fabs(a3_pos.x() - -6.7911f) < EP);
		TS_ASSERT(fabs(a3_pos.y() - 1.63313f) < EP);
		TS_ASSERT(fabs(a3_pos.z() - -0.462462f) < EP);
		std::cout << "a3 pos: " << a3_pos.x() << " " << a3_pos.y() << " " << a3_pos.z() << std::endl;

		// Check the rotation
		TS_ASSERT(fabs(a1->getPitch().getDegrees() - 5.0f) < EP);
		TS_ASSERT(fabs(a1->getYaw().getDegrees() - 15.0f) < EP);
		TS_ASSERT(fabs(a1->getRoll().getDegrees() - 25.0f) < EP);
		std::cout << "a1 rot: " << a1->getPitch().getDegrees() << " " << a1->getYaw().getDegrees() << " " << a1->getRoll().getDegrees() << std::endl;

		TS_ASSERT(fabs(a2->getPitch().getDegrees() - 3.98862f) < EP);
		TS_ASSERT(fabs(a2->getYaw().getDegrees() - 13.1276f) < EP);
		TS_ASSERT(fabs(a2->getRoll().getDegrees() - 7.01816f) < EP);
		std::cout << "a2 rot: " << a2->getPitch().getDegrees() << " " << a2->getYaw().getDegrees() << " " << a2->getRoll().getDegrees() << std::endl;

		TS_ASSERT(fabs(a3->getPitch().getDegrees() - 6.82211f) < EP);
		TS_ASSERT(fabs(a3->getYaw().getDegrees() - -9.04857f) < EP);
		TS_ASSERT(fabs(a3->getRoll().getDegrees() - -16.7055f) < EP);
		std::cout << "a3 rot: " << a3->getPitch().getDegrees() << " " << a3->getYaw().getDegrees() << " " << a3->getRoll().getDegrees() << std::endl;
	}

	// Test Detaching
	void test_ActorDetach() {
		// We have to set up g_grim first, which needs g_system
		TS_ASSERT(g_system != nullptr);
		Grim::GrimEngine g = Grim::GrimEngine(g_system, 0, Grim::GType_MONKEY4, Common::kPlatformWindows, Common::EN_ANY);
		Grim::g_grim = &g;

		Grim::Actor *a1 = new Grim::Actor();
		Grim::Actor *a2 = new Grim::Actor();
		Grim::Actor *a3 = new Grim::Actor();

		// Set the position and rotation for the actors
		Math::Vector3d a1_pv(1.0f, 4.0f, 5.0f);
		a1->setPos(a1_pv);
		a1->setRot(5.0f, 15.0f, 25.0f);

		Math::Vector3d a2_pv(3.0f, -2.0f, -1.0f);
		a2->setPos(a2_pv);
		a2->setRot(15.0f, 25.0f, 35.0f);

		Math::Vector3d a3_pv(-3.0f, -4.0f, 2.0f);
		a3->setPos(a3_pv);
		a3->setRot(12.0f, 16.0f, 18.0f);

		// Attach a2 to a1
		a2->attachToActor(a1, nullptr);

		// Attach a3 to a2
		a3->attachToActor(a2, nullptr);

		// Detach a3 from a2 and check the position and rotation
		a3->detach();

		Math::Vector3d a3_wpos = a3->getWorldPos();
		TS_ASSERT(fabs(a3_wpos.x() - -3.0f) < EP);
		TS_ASSERT(fabs(a3_wpos.y() - -4.0f) < EP);
		TS_ASSERT(fabs(a3_wpos.z() - 2.0f) < EP);
		std::cout << "a3 wpos: " << a3_wpos.x() << " " << a3_wpos.y() << " " << a3_wpos.z() << std::endl;

		Math::Vector3d a3_pos = a3->getPos();
		TS_ASSERT(fabs(a3_pos.x() - -3.0f) < EP);
		TS_ASSERT(fabs(a3_pos.y() - -4.0f) < EP);
		TS_ASSERT(fabs(a3_pos.z() - 2.0f) < EP);
		std::cout << "a3 pos: " << a3_pos.x() << " " << a3_pos.y() << " " << a3_pos.z() << std::endl;

		TS_ASSERT(fabs(a3->getPitch().getDegrees() - 9.0768f) < EP);
		TS_ASSERT(fabs(a3->getYaw().getDegrees() - 20.2983f) < EP);
		TS_ASSERT(fabs(a3->getRoll().getDegrees() - 14.6927f) < EP);
		std::cout << "a3 rot: " << a3->getPitch().getDegrees() << " " << a3->getYaw().getDegrees() << " " << a3->getRoll().getDegrees() << std::endl;

		// Now detach a2 from a1 and check that
		a2->detach();

		Math::Vector3d a2_wpos = a2->getWorldPos();
		TS_ASSERT(fabs(a2_wpos.x() - 3.0f) < EP);
		TS_ASSERT(fabs(a2_wpos.y() - -2.0f) < EP);
		TS_ASSERT(fabs(a2_wpos.z() - -1.0f) < EP);
		std::cout << "a2 wpos: " << a2_wpos.x() << " " << a2_wpos.y() << " " << a2_wpos.z() << std::endl;

		Math::Vector3d a2_pos = a2->getPos();
		TS_ASSERT(fabs(a2_pos.x() - 3.0f) < EP);
		TS_ASSERT(fabs(a2_pos.y() - -2.0f) < EP);
		TS_ASSERT(fabs(a2_pos.z() - -1.0f) < EP);
		std::cout << "a2 pos: " << a2_pos.x() << " " << a2_pos.y() << " " << a2_pos.z() << std::endl;

		TS_ASSERT(fabs(a2->getPitch().getDegrees() - 11.4362f) < EP);
		TS_ASSERT(fabs(a2->getYaw().getDegrees() - 27.3029f) < EP);
		TS_ASSERT(fabs(a2->getRoll().getDegrees() - 33.956f) < EP);
		std::cout << "a2 rot: " << a2->getPitch().getDegrees() << " " << a2->getYaw().getDegrees() << " " << a2->getRoll().getDegrees() << std::endl;
	}

	// Test rotation matrix
	void test_ActorFinalMatrix() {
		// We have to set up g_grim first, which needs g_system
		TS_ASSERT(g_system != nullptr);
		Grim::GrimEngine g = Grim::GrimEngine(g_system, 0, Grim::GType_MONKEY4, Common::kPlatformWindows, Common::EN_ANY);
		Grim::g_grim = &g;

		Grim::Actor *a1 = new Grim::Actor();
		Grim::Actor *a2 = new Grim::Actor();
		Grim::Actor *a3 = new Grim::Actor();

		// Set the position and rotation for the actors
		Math::Vector3d a1_pv(1.0f, 4.0f, 5.0f);
		a1->setPos(a1_pv);
		a1->setRot(5.0f, 15.0f, 25.0f);

		// Get the final matrix
		Math::Matrix4 m = a3->getFinalMatrix();

		// Get the euler angles from the matrix
		Math::Angle mp, my, mr;
//		m.getXYZ(&mr, &my, &mp, Math::EO_XZY);
		m.getXYZ(&mr, &my, &mp, Math::EO_ZYX);

		// Get the final quaternion
		Math::Quaternion q = a3->getRotationQuat();
		Math::Angle qp, qy, qr;
		q.toXYZ(&qr, &qy, &qp, Math::EO_XZY);

		TS_ASSERT(fabs(qp.getDegrees() - mp.getDegrees()) < EP);
		TS_ASSERT(fabs(qy.getDegrees() - my.getDegrees()) < EP);
		TS_ASSERT(fabs(qr.getDegrees() - mr.getDegrees()) < EP);


	}
};
#undef EP
