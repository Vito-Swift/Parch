/**
 * @filename: utils.hh.h
 * @author: Vito Wu <chenhaowu[at]link.cuhk.edu.cn>
 * @version: 
 * @desc:
 * @date: 2/22/2021
 */

#ifndef PARCH_UTILS_HH
#define PARCH_UTILS_HH

#define TIMEVAL2F(stamp) \
    ((stamp).tv_sec * 1000.0 + (stamp).tv_usec / 1000.0)

double get_timestamp() {

}

#endif //PARCH_UTILS_HH
