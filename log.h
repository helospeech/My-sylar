#ifndef _SYLAR_LOG_H
#define _SYLAR_LOG_H

#include<string>
#include<stdint.h>  //int32等
#include<memory>  //智能指针与内存管理
#include<list>//双向链表
#include<sstream>//stringStream
#include<fstream>//fileStream
#include<iostream>
#include<vector>
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

    //日志级别
    class logLevel{
    public:
        enum Level{
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
        //将logEvent format成string提供给logAppender
        std::string format(logEvent::ptr event);
        logFormatter(const std::string& pattern);
    
    private:
        class formatItem{
        public:
            typedef std::shared_ptr<formatItem> ptr;
            virtual ~formatItem(){}
            virtual void format(std::ostream& os,logEvent::ptr event) = 0;                
        };
        void init();
        std::string m_pattern;
        std::vector<formatItem::ptr> m_items;
    };

    //日志输出地
    class logAppender{
    public:
        typedef std::shared_ptr<logAppender> ptr;
        virtual ~logAppender(){}//定义虚析构函数，确保子类通过父类接口析构时能正确调用析构函数（不是虚的话会调用父类的析构函数）

        virtual void log(logLevel::Level level,logEvent::ptr event) = 0;//纯虚函数，子类必须实现
        void setFormatter(logFormatter::ptr formatter){m_formatter = formatter;}
        logFormatter::ptr getFormatter() const {return m_formatter;}
    protected:
        logLevel::Level m_level;
        logFormatter::ptr m_formatter;
    };

    //日志器
    class logger{
    public:
        typedef std::shared_ptr<logger> ptr;
        logger(const std::string& m_name = "root");

        void log(logLevel::Level level,logEvent::ptr event);

        void debug(logEvent::ptr event);//输出不同级别的日志
        void info(logEvent::ptr event);
        void warn(logEvent::ptr event);
        void error(logEvent::ptr event);
        void fatal(logEvent::ptr event);

        void addAppender(logAppender::ptr appender);
        void delAppender(logAppender::ptr appender);

        logLevel::Level get_level() const {return m_level;}//在类声明中直接实现函数，可以减少编译时间并提高可读性，这些成员函数通常会被编译器内联，（const保证不修改类中的变量）
        void set_level(logLevel::Level level){m_level = level;}//尽管内联并不是保证的，除非显式地使用inline关键字。内联会使函数在编译过程中在其调用处展开。
    private:
        std::string m_name;  //日志名称
        logLevel::Level m_level;  //日志级别
        std::list<logAppender::ptr> m_appenders;  //Appender集合
    };

    //输出到控制台
    class stdOutLogAppender: public logAppender{
    public:
        typedef std::shared_ptr<stdOutLogAppender> ptr;
        void log(logLevel::Level level,logEvent::ptr event) override;//显式声明一个成员函数是覆盖基类中的虚函数。
    private:
    };
    //输出到文件
    class fileLogAppender: public logAppender{
        typedef std::shared_ptr<fileLogAppender> ptr;
        fileLogAppender(const std::string& file_name);
        /*1.减少复制：如果使用值传递，传递给函数的对象会被复制到新的位置。对于大型对象，这可能会导致不必要的性能开销。通过引用传递，我们可以避免这种复制，因为引用仅仅是原始对象的一个别名。
        2.兼容性：使用引用传递，函数或方法可以接受调用者传递的临时对象。例如，你可以这样调用fileLogAppender构造函数：fileLogAppender("log.txt");。如果使用值传递，这样的调用就会失败，因为非常量引用不能绑定到临时对象。
        */ 
        void log(logLevel::Level level,logEvent::ptr event) override;//如果标记了override的成员函数没有正确地覆盖基类中的任何虚函数（例如，因为签名不匹配或基类函数不是虚函数），编译器将报错。这有助于避免因拼写错误、参数类型不匹配等问题而导致的错误。
        bool reopen();//重新打开文件
    
    private:
        std::string m_filename;
        std::ofstream m_fileStream;
    };
}

#endif