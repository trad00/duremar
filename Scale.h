#ifndef SCALE_H
#define SCALE_H

#include "HX711.h"

namespace scale {

HX711 scale;

void start() {
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(config::conf["scaleScale/1"]);
  scale.set_offset(config::conf["scaleOffset/1"]);
}
  
float readWeight() {
  float weight = emptySignal;
  if (scale.wait_ready_timeout(100)) {
    weight = scale.get_units(1);
    weight = round(weight);
//    if (weight < 0.0)
//      weight = 0.0;
  }
  return weight;
}

void resetScale(String dataPath) {
  if (scale.wait_ready_timeout(100)) {
    scale.set_scale();
    float weight = scale.get_units(5);
    int standard = config::conf["scaleStandard/" + dataPath];
    float newScale = weight / (float)standard;
    config::conf["scaleScale/" + dataPath] = newScale;
    scale.set_scale(newScale);
  }
}

void scaleTare() {
  String dataPath = "1";
  if (scale.wait_ready_timeout(100)) {
    scale.tare(5);
    config::conf["scaleOffset/" + dataPath] = scale.get_offset();
    config::saveConfig();
  }
}

} //namespace

#endif
