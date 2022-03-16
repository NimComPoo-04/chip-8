#ifndef _LOG_H_
#define _LOG_H_

#define LOG(msg, ...) __panik(0, __FILE__, __func__, __LINE__, msg,##__VA_ARGS__)
#define BOI(msg, ...) __panik(1, __FILE__, __func__, __LINE__, msg,##__VA_ARGS__)
#define ERR(msg, ...) __panik(2, __FILE__, __func__, __LINE__, msg,##__VA_ARGS__)
#define WTF(msg, ...) __panik(3, __FILE__, __func__, __LINE__, msg,##__VA_ARGS__)

void __panik(int threat_lvl, const char *file, const char *func, int line_no, const char *msg, ...);

#endif
