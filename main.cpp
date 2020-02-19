#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <sched.h>

using namespace std;

struct Mut{
    pthread_mutex_t* mutex;
    pthread_cond_t* cond;
    string name;
};

int var = 0;

void* wait_high(void* args){
    Mut* mut = (Mut*)args;
    int counter = 0;
    int spatus_counter = 0;
    while(1){
        pthread_mutex_lock(mut->mutex);
        while (var != 0) {
            pthread_cond_wait(mut->cond, mut->mutex);
            if (var == 100){
                var = 1;
                pthread_mutex_unlock(mut->mutex);
                cout << "HIGH counter = " << counter << " spatus = " << spatus_counter <<  endl;
                return NULL;
            }
            if (var != 0){
                spatus_counter++;
            }
        }
        var = 1;
        counter++;
        pthread_mutex_unlock(mut->mutex);
    }
    return NULL;
}

void* wait_low(void* args) {
    Mut *mut = (Mut *) args;
    sched_param param;
    param.sched_priority = 0;
    pthread_setschedparam(pthread_self(), SCHED_RR, &param);
    int counter = 0;
    int spatus_counter = 0;
    while (1) {
        pthread_mutex_lock(mut->mutex);
        while (var != 0) {
            pthread_cond_wait(mut->cond, mut->mutex);
            if (var == 100){
                var = 1;
                pthread_mutex_unlock(mut->mutex);
                cout << "LOW counter = " << counter << " spatus = " << spatus_counter <<  endl;
                return NULL;
            }
            if (var != 0){
                spatus_counter++;
            }
        }
        var = 1;
        counter++;
        pthread_mutex_unlock(mut->mutex);
    }
    return NULL;
}

void* make_signal(void* args){
    Mut* mut = (Mut*)args;
    int counter = 0;
    while(counter<500000) {
        pthread_mutex_lock(mut->mutex);
        var = 0;
//        pthread_mutex_unlock(mut->mutex);
        pthread_cond_signal(mut->cond);
        pthread_mutex_unlock(mut->mutex);
        usleep(1);
        counter++;
    }
    //FINISH threads
    for (int i = 0; i < 2; i++) {
        pthread_mutex_lock(mut->mutex);
        var = 100;
        pthread_cond_signal(mut->cond);
        pthread_mutex_unlock(mut->mutex);
        usleep(100);
    }

    return NULL;
}


int main() {
    pthread_cond_t m_startConvertingCondition;
    pthread_mutex_t m_startConvertingMutex;
    pthread_mutex_init(&m_startConvertingMutex, NULL);
    pthread_cond_init(&m_startConvertingCondition, NULL);
    pthread_attr_t tattr;
    sched_param param_l, param_h;
    Mut mut;
    mut.mutex = &m_startConvertingMutex;
    mut.cond = &m_startConvertingCondition;
    pthread_t wait_t_low, wait_t_high, signal_t;

    pthread_create(&wait_t_low, NULL,
                   wait_low, &mut);

    pthread_create(&wait_t_high, NULL,
                   wait_high, &mut);

    int ret_l, ret_h;
    usleep(300);
    ret_l = pthread_setschedprio(wait_t_low, 0);
    usleep(400);
    ret_h = pthread_setschedprio(wait_t_high, 1);
    cout << ret_l << " " << ret_h << endl;

    usleep(200);
    pthread_create(&signal_t, NULL,
                   make_signal, &mut);

    sleep(4000);
    std::cout<<"Finish!" << std::endl;
    return 0;
}
