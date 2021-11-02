#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H


#include <deque>
#include <queue>
#include <mutex>
#include <vector>
using std::mutex;
using std::vector;

class DataSaver;
class Dealer;
class Detect_Service{
public:
    virtual ~Detect_Service() = 0;
    virtual void bind(Dealer * dealer) = 0;
    virtual void onReceive(vector<double> segData) = 0;
    virtual void onReceive(float x, float y) = 0;
};

#define NumOfSession 3
class Dealer{
protected:
    static Dealer * instance;
    Dealer();
    mutex write_mtx;
    vector<DataSaver*> Catter;
    vector<vector<float>> SegData;
    vector<mutex> IfSeg;

public:
    static Dealer * getInstance(){
        if(instance == nullptr){
            instance = new Dealer();
        }
        return instance;
    }

    size_t INIT(DataSaver *unit) {
        if(unit == nullptr) return -1;
        size_t index;
        mutex * m_tmp = new mutex();
        write_mtx.lock();

        //////// 初始化注册资格 ////////
        // 注册下标 ID
        index = Catter.size();
        // 插入Saver指针
        Catter.push_back(unit);
        // 注册插入新的切断数据
        SegData.push_back(vector<float>());
        //////////////////////////////

        write_mtx.unlock();
        IfSeg.push_back(*m_tmp);

        return index;
    }

    size_t UnRegister(DataSaver *unit, int ID) {
        write_mtx.lock();
        if(unit == Catter[ID]){
            Catter.erase(Catter.begin() + ID);
            SegData.erase(SegData.begin() + ID);
            IfSeg.erase(IfSeg.begin() + ID);
        }
        write_mtx.unlock();
    }
    int RECEIVE_SEG(vector<float> Data, int ID){
//        TODO: 需要确定更好的方法
        write_mtx.lock();

        write_mtx.unlock();
    }

};

// 设置缓存最长长度
#define MAXLEN 15000

class DataSaver {
// ********* DataSaver ********** //
protected:
    // 双端队列存储数据
    std::deque<float> databuff;
    // 标注长度和下标集合
    size_t INDEX;
    std::queue<size_t> spikes;

    //////////// Dealer service ///////////
    // 绑定服务
    Dealer *m_dealer;
    // 注册标识
    int ID;
    ///////////////////////////////////////

    // 写入锁
    mutex write_mutex;
public:
    DataSaver(Dealer *dealer) {
        m_dealer = dealer;
        INDEX = 0;
        // 注册服务
        ID = m_dealer->INIT(this);
    }
    // 回调服务程序
    void feedData(float *data, size_t dsize) {
        write_mutex.lock();
        for(size_t i = 0;i < dsize; i++){
            databuff.push_back(data[i]);
            if(databuff.size() > MAXLEN) databuff.pop_front();
            INDEX ++;
            if(INDEX < 200) continue;

            if(spikes.size() == 0 && (data[i] > 7 || data[i] < -7)) {
                spikes.push(INDEX);
            }
            if(spikes.size() != 0 && INDEX - spikes.front() - 1299 == 0 ){
                spikes.pop();
                vector<float> tmp;
//                m_dealer->RECEIVE_SEG()
            }
        }
        write_mutex.unlock();
    }
};

#endif // DATAPROCESSING_H
