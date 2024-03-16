#include "log.h"

namespace sylar{
logger::logger(const std::string& name)
    :m_name(name){//":"引入了成员初始化列表,成员初始化列表直接调用成员的构造函数来初始化成员变量，而不是先调用默认构造函数再通过赋值操作进行覆盖。
}
void logger::addAppender(logAppender::ptr appender){
    m_appenders.push_back(appender);
}
void logger::delAppender(logAppender::ptr appender){
    for(auto it = m_appenders.begin();it != m_appenders.end();++it){//auto使得很复杂的定义变得方便
        if(*it==appender){
            m_appenders.erase(it);//传递迭代器删除元素
            break;
        }
    }
}
void logger::log(logLevel::Level level,logEvent::ptr event){
    if(level>=m_level){//输出
        for(auto& i : m_appenders){//不是in,是:
            i->log(level,event);
        }
    }
}
//输出不同级别的日志
void logger::debug(logEvent::ptr event){
    log(logLevel::DEBUG,event);
}
void logger::info(logEvent::ptr event){
    log(logLevel::INFO,event);
}
void logger::warn(logEvent::ptr event){
    log(logLevel::WARN,event);
}
void logger::error(logEvent::ptr event){
    log(logLevel::ERROR,event);
}
void logger::fatal(logEvent::ptr event){
    log(logLevel::FATAL,event);
}
fileLogAppender::fileLogAppender(const std::string& filename)
    :m_filename(filename){

}
void stdOutLogAppender:: log(logLevel::Level level,logEvent::ptr event){
    if(level>=m_level){
        std::cout<<m_formatter->format(event);
    }
}
bool fileLogAppender::reopen(){
    if(m_fileStream){
        m_fileStream.close();
    }
    m_fileStream.open(m_filename);
    return !!m_fileStream;//两次取反，转换为bool值
}
void fileLogAppender:: log(logLevel::Level level,logEvent::ptr event){
    if(level>=m_level){
        m_fileStream<<m_formatter->format(event);//智能指针本质是类，其中会重写->和.等
    }
}
logFormatter::logFormatter(const std::string& pattern)
    :m_pattern(pattern){
}
std::string logFormatter::format(logEvent::ptr event){
    std::stringstream ss;
    for(auto& i : m_items){
        i->format(ss,event);
    }
    return ss.str();
}
void logFormatter::init(){
    std::vector<std::pair<std::string,int>> vec;
    std::string nstr;
    for(size_t i=0;i<m_pattern.size();i++){
        if(m_pattern[i]!='%'){
            nstr.append(1,m_pattern[i]);
            continue;
        }

        if((i+1)<m_pattern.size()&&m_pattern[i+1]=='%'){
            nstr.append(1,'%');
            continue;
        }
        size_t n =i+1;
        int fmt_status = 0;//接受格式
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n<m_pattern.size()){
            if(isspace(m_pattern[n])){
                break;
            }
            if(fmt_status == 0){
                if(m_pattern[n]=='{'){
                    str = m_pattern.substr(i+1,n-i);
                    fmt_status = 1;//解析格式
                    fmt_begin = ++n;
                    continue;
                }
            }
            if(fmt_status == 1){
                if(m_pattern[n]=='}'){
                    fmt = m_pattern.substr(fmt_begin+1,n-fmt_begin);
                    fmt_status = 2;
                    break;
                }
            }
            
        }
        if(fmt_status == 0){
            if(!nstr.empty()){
                vec.push_back(std::make_pair(nstr,"",0));
            }
            str = m_pattern.substr(i+1,n-i-1);
            vec.push_back(std::make_tuple(str,fmt,1));
            i = n;
        }else if(fmt_status == 1){
            std::cout << "pattern parse error:" << m_pattern << "-" << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>",fmt,1));
        }else if(fmt_status == 2){
            if(!nstr.empty()){
                vec.push_back(std::make_pair(nstr,"",0));
            }
            vec.push_back(std::make_tuple(str,fmt,1));
            i = n;
        }
    }
    if(!nstr.empty()){
        vec.push_back(std::make_pair(nstr,"",0));
    }

    //%m - 消息体
}
}