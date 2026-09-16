#include <frc/TimedRobot.h>
#include <frc/XboxController.h>
#include <rev/SparkMax.h>
#include <rev/config/SparkMaxConfig.h>

class Robot : public frc::TimedRobot {
public:
  // 모터 4개 선언 (왼쪽: 8, 11 / 오른쪽: 2, 18)
  rev::spark::SparkMax m_leftLeader{8, rev::spark::SparkMax::MotorType::kBrushless};
  rev::spark::SparkMax m_leftFollower{11, rev::spark::SparkMax::MotorType::kBrushless}; 

  rev::spark::SparkMax m_rightLeader{2, rev::spark::SparkMax::MotorType::kBrushless};
  rev::spark::SparkMax m_rightFollower{18, rev::spark::SparkMax::MotorType::kBrushless};

  // 추가: 장전/발사 모터 (CAN ID 15), 급탄 모터 (CAN ID 16)
  rev::spark::SparkMax m_shooter{15, rev::spark::SparkMax::MotorType::kBrushless};
  rev::spark::SparkMax m_feeder{16, rev::spark::SparkMax::MotorType::kBrushless};

  // 로지텍 F310 조종기 (포트 0번)
  frc::XboxController m_controller{0};

  void RobotInit() override {
    // 2026년형 설정 객체 생성
    rev::spark::SparkMaxConfig leftLeaderConfig;
    rev::spark::SparkMaxConfig leftFollowerConfig;
    rev::spark::SparkMaxConfig rightLeaderConfig;
    rev::spark::SparkMaxConfig rightFollowerConfig;

    
    // 추가: 슈터 및 급탄 모터 설정 객체 생성
    rev::spark::SparkMaxConfig shooterConfig;
    rev::spark::SparkMaxConfig feederConfig;

    // 양쪽 다 동일한 방향(false)으로 통일하여 조이스틱을 밀었을 때 같이 전진하도록 수정
    leftLeaderConfig.Inverted(true);
    rightLeaderConfig.Inverted(false);

    // 팔로워P 모터가 각각 리더 모터를 따르도록 설정
    leftFollowerConfig.Follow(m_leftLeader);
    rightFollowerConfig.Follow(m_rightLeader);

    // 모터에 설정값 적용
    m_leftLeader.Configure(leftLeaderConfig, rev::spark::SparkMax::ResetMode::kResetSafeParameters, rev::spark::SparkMax::PersistMode::kPersistParameters);
    m_leftFollower.Configure(leftFollowerConfig, rev::spark::SparkMax::ResetMode::kResetSafeParameters, rev::spark::SparkMax::PersistMode::kPersistParameters);
    
    m_rightLeader.Configure(rightLeaderConfig, rev::spark::SparkMax::ResetMode::kResetSafeParameters, rev::spark::SparkMax::PersistMode::kPersistParameters);
    m_rightFollower.Configure(rightFollowerConfig, rev::spark::SparkMax::ResetMode::kResetSafeParameters, rev::spark::SparkMax::PersistMode::kPersistParameters);

    // 추가: 슈터 및 급탄 모터 설정 적용
    m_shooter.Configure(shooterConfig, rev::spark::SparkMax::ResetMode::kResetSafeParameters, rev::spark::SparkMax::PersistMode::kPersistParameters);
    m_feeder.Configure(feederConfig, rev::spark::SparkMax::ResetMode::kResetSafeParameters, rev::spark::SparkMax::PersistMode::kPersistParameters);

    //전류제한 걸어놓기 
    //m_shooter.SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);
    //m_feeder.SetIdleMode(rev::CANSparkMax::IdleMode::kCoast);

  }

  void RobotPeriodic() override {}

  void AutonomousInit() override {}
  void AutonomousPeriodic() override {}

  void TeleopInit() override {}
  

  void TeleopPeriodic() override {
    // 로지텍 F310 조종기: 왼쪽 스틱 Y축 = 좌측 바퀴, 오른쪽 스틱 Y축 = 우측 바퀴
    // 최대 속도를 절반(0.1)으로 제한합니다
    // 조심히 (속도)
    double leftSpeed = m_controller.GetLeftY() * 0.1;
    double rightSpeed = m_controller.GetRightY() * 0.1;

    m_leftLeader.Set(leftSpeed);
    m_rightLeader.Set(rightSpeed);

    // 추가: X 버튼을 누르고 있는 동안 CAN ID 15번(장전/발사) 모터 구동 (속도 계수 0.1 적용)
    if (m_controller.GetXButton()) {
      m_shooter.Set(1.0);
    } else if(m_controller.GetAButton()){
      m_shooter.Set(-1.0);
    }
    else{
      m_shooter.Set(0.0);
    }


    // 추가: Y 버튼을 누르고 있는 동안 CAN ID 16번(급탄) 모터 구동 (속도 계수 0.1 적용)
    if (m_controller.GetYButton()) {
      m_feeder.Set(1);
    }else if(m_controller.GetBButton()){
      m_feeder.Set(-1);
    }
     else {
      m_feeder.Set(0.0);
    }
  }

  void DisabledInit() override {}
  void DisabledPeriodic() override {}
};

#ifndef RUNNING_FRC_TESTS
int main() {
  return frc::StartRobot<Robot>();
}
#endif