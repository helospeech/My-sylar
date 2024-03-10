#ifndef _SYLAR_LOG_H
#define _SYLAR_LOG_H

#include<string>
#include<stdint.h>  //int32等
#include<memory>  //智能指针与内存管理
namespace sylar{
    //每个日志生成时注册成logEvent,日志事件
    class logEvent{
    public:
        typedef std::shared_ptr<logEvent> ptr; //智能指针
        logEvent();
    private:
        const char* m_file = nullptr;//文件名
        int32_t m_line = 0;//行号
        uint32_t m_elapse = 0;//程序启动到现在的毫秒数
        uint32_t mthreadId = 0;//线程id
        uint32_t fiberId = 0;//协程id
        uint64_t m_time = 0;//时间戳
        std::string m_content = "";//消息内容
    };

    class logLevel{
    public:
        enum Level{//日志级别
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };
    };

    //定义日志格式
    class logFormatter{
    public:
        typedef std::shared_ptr<logFormatter> ptr;
        std::string format(logEvent::ptr event);//将logEvent format成string提供给logAppender
    };

    //日志输出地
    class logAppender{
    public:
        typedef std::shared_ptr<logAppender> ptr;
        virtual ~logAppender(){}//定义虚析构函数，确保子类通过父类接口析构时能正确调用析构函数（不是虚的话会调用父类的析构函数）

        void log(logLevel::Level level,logEvent::ptr event);
    private:
        logLevel::Level m_level;
    };

    //日志器
    class logger{
    public:
        typedef std::shared_ptr<logger> ptr;
        logger(const std::string& m_name = "root");

        void log(logLevel::Level level,logEvent::ptr event);
    private:
        std::string m_name;
        logLevel::Level m_level;
        logAppender::ptr appender;
    };

    //输出到控制台
    class stdOutLogAppender: public logAppender{};
    //输出到文件
    class fileLogAppender: public logAppender{};
}

#endif