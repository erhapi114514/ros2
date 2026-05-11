#include <QApplication>
#include <QLabel>
#include <QString>
#include "rclcpp/rclcpp.hpp"
#include "status_interfaces/msg/system_status.hpp"

using SystemStatus = status_interfaces::msg::SystemStatus;

class SysStatusDisplay : public rclcpp::Node {
 public:
  SysStatusDisplay() : Node("sys_status_display") {
    subscription_ = this->create_subscription<SystemStatus>(
        "sys_status", 10, [&](const SystemStatus::SharedPtr msg) -> void {
          label_->setText(get_qstr_from_msg(msg));
        });
  	// 创建一个空的 SystemStatus 对象，转化成 QString 进行显示
    label_ = new QLabel(get_qstr_from_msg(std::make_shared<SystemStatus>()));
    label_->show();
  }
QString get_qstr_from_msg(const SystemStatus::SharedPtr msg) {
    std::stringstream show_str;
    show_str
        << "=========== System Status Monitor ============\n"
        << "Timestamp:\t" << msg->stamp.sec << "\ts\n"
        << "Host Name:\t" << msg->host_name << "\t\n"
        << "CPU Usage:\t" << msg->cpu_percent << "\t%\n"
        << "Memory Usage:\t" << msg->memory_percent << "\t%\n"
        << "Memory Total:\t" << msg->memory_total << "\tMB\n"
        << "Memory Avail:\t" << msg->memory_available << "\tMB\n"
        << "Net Sent:\t" << msg->net_sent << "\tMB\n"
        << "Net Recv:\t" << msg->net_recv << "\tMB\n"
        << "==========================================";

    return QString::fromStdString(show_str.str());
  }


 private:
  rclcpp::Subscription<SystemStatus>::SharedPtr subscription_;
  QLabel* label_;
};


int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  QApplication app(argc, argv);
  auto node = std::make_shared<SysStatusDisplay>();
  std::thread spin_thread([&]() -> void { rclcpp::spin(node); });
  spin_thread.detach();
  app.exec();
  rclcpp::shutdown();
  return 0;
}
