#include "config_file.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

bool loadSensorConfig(ExcavatorConfig *config, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return false;
    
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        int index;
        int id;
        char axis;
        int inverted, points_down;
        double offset;
        int length;
        
        if (sscanf(line, "sensor %d id=%d axis=%c inverted=%d points_down=%d offset=%lf length=%d",
                   &index, &id, &axis, &inverted, &points_down, &offset, &length) == 7) {
            if (index >= 0 && index < 6) {
                config->sensors[index].id = id;
                config->sensors[index].axis = (axis == 'X') ? MountAxis::X : MountAxis::Y;
                config->sensors[index].inverted = inverted != 0;
                config->sensors[index].points_down = points_down != 0;
                config->sensors[index].offset = offset;
                config->sensors[index].length_mm = length;
            }
        }
    }
    
    fclose(f);
    return true;
}

bool saveSensorConfig(const ExcavatorConfig *config, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) return false;
    
    for (int i = 0; i < 6; i++) {
        const SensorConfig &s = config->sensors[i];
        fprintf(f, "sensor %d id=%d axis=%c inverted=%d points_down=%d offset=%.1f length=%d\n",
                i, s.id, (s.axis == MountAxis::X) ? 'X' : 'Y',
                s.inverted ? 1 : 0, s.points_down ? 1 : 0,
                s.offset, s.length_mm);
    }
    
    fclose(f);
    return true;
}
