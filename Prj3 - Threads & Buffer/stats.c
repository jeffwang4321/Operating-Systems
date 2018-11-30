#include "stats.h"

#include <stdlib.h>
#include <stdio.h>

int factoryNum = 0;

typedef struct {
    int num, made, eaten;
    double mindelay, maxdelay, avgdelay;
}statistics;

statistics* stats = NULL;

void stats_init(int num_producers){
  factoryNum = num_producers;
  stats = (statistics *) realloc (stats, num_producers * sizeof(statistics));

  for (int i = 0; i < num_producers; i++){
           stats[i].num = i;
           stats[i].made = 0;
           stats[i].eaten = 0;
           stats[i].mindelay = 9.0;
           stats[i].maxdelay = 0.0;
           stats[i].avgdelay = 0.0;
         }
}

void stats_cleanup(void)
{
    free(stats);
    return;
}

void stats_record_produced(int factory_number)
{
    stats[factory_number].made++;
    return;
}

void stats_record_consumed(int factory_number, double delay_in_ms)
{
    stats[factory_number].eaten++;

    if (delay_in_ms < stats[factory_number].mindelay)
       stats[factory_number].mindelay = delay_in_ms;
   if (delay_in_ms > stats[factory_number].maxdelay)
       stats[factory_number].maxdelay = delay_in_ms;
   stats[factory_number].avgdelay += delay_in_ms;

   return;
}

void stats_display()
{

    printf("%8s%10s%10s%18s%18s%18s\n","Factory#", "#Made", "#Eaten", "Min Delay[ms]", "Avg Delay[ms]", "Max Delay[ms]");
    for (int i = 0; i < factoryNum; i++) {

        stats[i].avgdelay = stats[i].avgdelay / stats[i].made;
        printf("%4d", stats[i].num);
        printf("%12d", stats[i].made);
        printf("%10d", stats[i].eaten);
        printf("%19.5f", stats[i].mindelay);
        printf("%18.5f", stats[i].avgdelay);
        printf("%18.5f", stats[i].maxdelay);
        printf("\n");

        if (stats[i].made != stats[i].eaten) {
            printf("Error: Mismatch between number made and eaten.\n");
            break;
        }
    }

    return;
}
