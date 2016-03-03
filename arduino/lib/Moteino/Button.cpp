#include <Button.h>

Button::Button(){
}

Button::~Button(){
}

void Button::init(uint8_t btn_pin, unsigned long idle_delay){
  m_btn_pin=btn_pin;
  m_idle_delay=idle_delay;
  pinMode(m_btn_pin, INPUT);
}

void Button::check() {
  bool val = digitalRead(m_btn_pin);
  unsigned long time = millis();
  if(val!=last_val) {
    write=true;
    if(!val){//end of push
      unsigned long delta =(time-last_switch_time)/BTN_PUSH_GRAN;
      if(delta<=0) delta=1;
      (writeBackBuffer?bck_buffer:frt_buffer)[series_nb_vals]=delta;
      series_nb_vals++;
      if(series_nb_vals>=BTN_MAX_PUSHES) series_nb_vals=0;
    }
    last_switch_time=time;
    last_val=val;
  } else if (!val && write && time>last_switch_time+m_idle_delay){
    (writeBackBuffer?bck_buffer:frt_buffer)[series_nb_vals]=0;
    series_nb_vals=0;
    writeBackBuffer=!writeBackBuffer;
    write=false;
    m_has_series=true;
    last_val=0;
  }
}

int Button::anaRead(){
  return analogRead(m_btn_pin);
}

bool Button::hasSeries(){
  return m_has_series;
}

byte *Button::pop(){
  if(!m_has_series) return 0;
  m_has_series=false;
  return writeBackBuffer?frt_buffer:bck_buffer;
}
