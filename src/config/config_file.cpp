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

bool loadPositions(StoredPosition *positions, int maxPositions, int *selectedPosition, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return false;
    
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        int index;
        int occupied;
        double depth, reach;
        
        // Try to parse position line
        if (sscanf(line, "position %d occupied=%d depth=%lf reach=%lf",
                   &index, &occupied, &depth, &reach) == 4) {
            if (index >= 0 && index < maxPositions) {
                positions[index].occupied = occupied != 0;
                positions[index].depth = depth;
                positions[index].reach = reach;
                
                // Parse sensor data from rest of line
                const char *p = strstr(line, "sensors=");
                if (p && positions[index].occupied) {
                    p += 8;  // skip "sensors="
                    for (int i = 0; i < NUM_SENSORS; i++) {
                        int id, connected;
                        double x, y;
                        if (sscanf(p, "%d,%lf,%lf,%d", &id, &x, &y, &connected) == 4) {
                            positions[index].sensors[i].id = id;
                            positions[index].sensors[i].x = x;
                            positions[index].sensors[i].y = y;
                            positions[index].sensors[i].connected = connected != 0;
                        }
                        // Move to next sensor (skip past semicolon)
                        const char *next = strchr(p, ';');
                        if (next) p = next + 1;
                        else break;
                    }
                }
            }
        }
        
        // Parse selected position
        int sel;
        if (sscanf(line, "selected=%d", &sel) == 1) {
            *selectedPosition = sel;
        }
    }
    
    fclose(f);
    return true;
}

bool savePositions(const StoredPosition *positions, int maxPositions, int selectedPosition, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) return false;
    
    for (int i = 0; i < maxPositions; i++) {
        const StoredPosition &pos = positions[i];
        fprintf(f, "position %d occupied=%d depth=%.1f reach=%.1f sensors=",
                i, pos.occupied ? 1 : 0, pos.depth, pos.reach);
        
        for (int j = 0; j < NUM_SENSORS; j++) {
            const Sensor &s = pos.sensors[j];
            fprintf(f, "%d,%.2f,%.2f,%d", s.id, s.x, s.y, s.connected ? 1 : 0);
            if (j < NUM_SENSORS - 1) fprintf(f, ";");
        }
        fprintf(f, "\n");
    }
    
    fprintf(f, "selected=%d\n", selectedPosition);
    
    fclose(f);
    return true;
}
