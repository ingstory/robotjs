#ifndef POWER_MANAGEMENT_H
#define POWER_MANAGEMENT_H

#define POWER_STATE_SUSPEND 0
#define POWER_STATE_HIBERNATE 1
#define POWER_STATE_SHUTDOWN 2
#define POWER_STATE_FORCE_SHUTDOWN 3

#ifdef __cplusplus
extern "C" {
#endif

void setSystemPowerState(int state);
int getSystemPowerState(void);
int deleteProgram(const char* programPath);
char** getInstalledPrograms(int* count);
void freeInstalledPrograms(char** programs, int count);

#ifdef __cplusplus
}
#endif

#endif /* POWER_MANAGEMENT_H */