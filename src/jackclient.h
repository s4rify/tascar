/**
   \file jackclient.h
   \ingroup libtascar
   \brief "jackclient" is a C++ wrapper for the JACK API.
   \author Giso Grimm
   \date 2011

   \section license License (LGPL)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; version 2 of the
   License.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

*/

#ifndef JACKCLIENT_H
#define JACKCLIENT_H

#include <jack/jack.h>
#include <string>
#include <vector>

class jackc_portless_t {
public:
  jackc_portless_t(const std::string& clientname);
  ~jackc_portless_t();
  void tp_start();
  void tp_stop();
  void tp_locate(uint32_t p);
  void tp_locate(double t_sec);
  uint32_t tp_get_frame() const;
  double tp_get_time() const;
  void activate();
  void deactivate();
  std::string get_client_name();
  int get_srate() { return srate; };
  int get_fragsize() { return fragsize; };
  void connect(const std::string& src, const std::string& dest, bool btry=false);
protected:
  jack_client_t* jc;
  int srate;
  int fragsize;
  int rtprio;
  bool active;
};

/**
   \brief Jack client C++ wrapper
   \ingroup libtascar
*/
class jackc_t : public jackc_portless_t {
public:
  jackc_t(const std::string& clientname);
  virtual ~jackc_t();
  virtual void add_input_port(const std::string& name);
  virtual void add_output_port(const std::string& name);
  void connect_in(unsigned int port,const std::string& pname,bool btry=false);
  void connect_out(unsigned int port,const std::string& pname,bool btry=false);
  size_t get_num_input_ports() const {return inPort.size();};
  size_t get_num_output_ports() const {return outPort.size();};
protected:
  virtual int process(jack_nframes_t nframes,const std::vector<float*>& inBuffer,const std::vector<float*>& outBuffer) = 0;
private:
  static int process_(jack_nframes_t nframes, void *arg);
  int process_(jack_nframes_t nframes);
private:
  std::vector<jack_port_t*> inPort;
  std::vector<jack_port_t*> outPort;
  std::vector<float*> inBuffer;
  std::vector<float*> outBuffer;
};

class jackc_db_t : public jackc_t {
public:
  jackc_db_t(const std::string& clientname,jack_nframes_t fragsize);
  virtual ~jackc_db_t();
  virtual void add_input_port(const std::string& name);
  virtual void add_output_port(const std::string& name);
protected:
  virtual int inner_process(jack_nframes_t nframes,const std::vector<float*>& inBuffer,const std::vector<float*>& outBuffer) = 0;
  virtual int process(jack_nframes_t nframes,const std::vector<float*>& inBuffer,const std::vector<float*>& outBuffer);
private:
  static void * service(void* h);
  void service();
  std::vector<float*> dbinBuffer[2];
  std::vector<float*> dboutBuffer[2];
  jack_nframes_t inner_fragsize;
  bool inner_is_larger;
  uint32_t ratio;
  jack_native_thread_t inner_thread;
  pthread_mutex_t mutex[2];
  pthread_mutex_t mtx_inner_thread;
  bool buffer_filled[2];
  uint32_t current_buffer;
  bool b_exit_thread;
  uint32_t inner_pos;
};

class jackc_transport_t : public jackc_t {
public:
  jackc_transport_t(const std::string& clientname);
protected:
  int process(jack_nframes_t nframes,const std::vector<float*>& inBuffer,const std::vector<float*>& outBuffer);
  virtual int process(jack_nframes_t nframes,const std::vector<float*>& inBuffer,const std::vector<float*>& outBuffer,uint32_t tp_frame, bool tp_running) = 0;
};

#endif

/*
 * Local Variables:
 * mode: c++
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * compile-command: "make -C .."
 * End:
 */

