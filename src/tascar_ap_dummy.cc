/*
 * (C) 2016 Giso Grimm
 * (C) 2015-2016 Gerard Llorach to
 *
 */

#include "audioplugin.h"
#include <lo/lo.h>
#include "stft.h"
#include <string.h>
#include "errorhandling.h"

class dummy_t : public TASCAR::audioplugin_base_t {
public:
  dummy_t( const TASCAR::audioplugin_cfg_t& cfg );
  void ap_process(TASCAR::wave_t& chunk, const TASCAR::pos_t& pos, const TASCAR::transport_t& tp);
  void prepare( double srate,uint32_t fragsize );
  void release();
  void add_variables( TASCAR::osc_server_t* srv );
  ~dummy_t();
private:
};

dummy_t::dummy_t( const TASCAR::audioplugin_cfg_t& cfg )
  : audioplugin_base_t( cfg )
{
  DEBUG("--constructor--");
  DEBUG(f_sample);
  DEBUG(f_fragment);
  DEBUG(t_sample);
  DEBUG(t_fragment);
  DEBUG(n_fragment);
  DEBUG(name);
  DEBUG(modname);
}

void dummy_t::add_variables( TASCAR::osc_server_t* srv )
{
  DEBUG(srv);
}

void dummy_t::prepare(double srate,uint32_t fragsize)
{
  DEBUG("--prepare--");
  DEBUG(f_sample);
  DEBUG(f_fragment);
  DEBUG(t_sample);
  DEBUG(t_fragment);
  DEBUG(n_fragment);
  DEBUG(name);
  DEBUG(modname);
  DEBUG(srate);
  DEBUG(fragsize);
}

void dummy_t::release()
{
  DEBUG("--release--");
}

dummy_t::~dummy_t()
{
  DEBUG("--destruct--");
}

void dummy_t::ap_process(TASCAR::wave_t& chunk, const TASCAR::pos_t& pos, const TASCAR::transport_t& tp)
{
  DEBUG(chunk.n);
}

REGISTER_AUDIOPLUGIN(dummy_t);

/*
 * Local Variables:
 * mode: c++
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * compile-command: "make -C .."
 * End:
 */
