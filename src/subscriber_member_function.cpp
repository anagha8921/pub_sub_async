#include <memory>

#include "rclcpp/rclcpp.hpp"
#include <thread>
#include "tutorial_interfaces/srv/shutdown.hpp"
#include "tutorial_interfaces/msg/service.hpp"
#include "std_msgs/msg/string.hpp"
using std::placeholders::_1;
void printThreadID() {
    std::thread::id threadId = std::this_thread::get_id();
    std::cout << "Thread ID: " << threadId << std::endl;
}
void response_received_callback(const rclcpp::Client<tutorial_interfaces::srv::Shutdown>::SharedFuture future)  // CHANGE
{
   RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Got the Brightness callback response!!!");
   auto result = future.get();
   RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Result: %d", result->resp.resp);
   printThreadID();
  
}
void response_received_callback_b(const rclcpp::Client<tutorial_interfaces::srv::Shutdown>::SharedFuture future)  // CHANGE
{
   RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Got the Contrast callbackresponse!!!");
   auto result = future.get();
   RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Result: %d", result->resp.resp);
   printThreadID();
  
}

class MinimalSubscriber : public rclcpp::Node
{
  public:
    MinimalSubscriber()
    : Node("minimal_subscriber")
    {
      subscription_ = this->create_subscription<std_msgs::msg::String>(
      "brightness", 10, std::bind(&MinimalSubscriber::topic_callback, this, _1));
      subscription_contrast = this->create_subscription<std_msgs::msg::String>(
      "contrast", 10, std::bind(&MinimalSubscriber::topic_callback_contrast, this, _1));
      
      client =  this->create_client<tutorial_interfaces::srv::Shutdown>("service_a"); 
      client_b =  this->create_client<tutorial_interfaces::srv::Shutdown>("service_b"); 
    }
  using ServiceResponseFuture =
      rclcpp::Client<tutorial_interfaces::srv::Shutdown>::SharedFuture; 
  private:
    void topic_callback(const std_msgs::msg::String::SharedPtr msg) const
    {
      RCLCPP_INFO(this->get_logger(), "Received Brightness update: '%s'", msg->data.c_str());
      
      auto request = std::make_shared<tutorial_interfaces::srv::Shutdown::Request>();               // CHANGE
      request->req = 0;// CHANGE
      using ServiceResponseFuture =
      rclcpp::Client<tutorial_interfaces::srv::Shutdown>::SharedFuture; 
      ServiceResponseFuture future;

      auto result = client->async_send_request(request,response_received_callback);
      RCLCPP_INFO(this->get_logger(), "Service request send from Brightness Callback");
      for (int i=0;i<3;i++){
        RCLCPP_INFO(this->get_logger(), "Inside Brightness Callback");
      }
      printThreadID();
    }
    void topic_callback_contrast(const std_msgs::msg::String::SharedPtr msg) const
    {
      RCLCPP_INFO(this->get_logger(), "Received Contrast update: '%s'", msg->data.c_str());


       auto request = std::make_shared<tutorial_interfaces::srv::Shutdown::Request>();               // CHANGE
      request->req = 0;// CHANGE
      using ServiceResponseFuture =
      rclcpp::Client<tutorial_interfaces::srv::Shutdown>::SharedFuture; 
      ServiceResponseFuture future;

      auto result = client_b->async_send_request(request,response_received_callback_b);
      RCLCPP_INFO(this->get_logger(), "Service request send from Contrast Callback");
      for (int i=0;i<3;i++){
        RCLCPP_INFO(this->get_logger(), "Inside Contrast Callback");
      }
      printThreadID();
    }
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_contrast;
    rclcpp::Client<tutorial_interfaces::srv::Shutdown>::SharedPtr  client;
    rclcpp::Client<tutorial_interfaces::srv::Shutdown>::SharedPtr  client_b;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();
  return 0;
}