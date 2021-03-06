/**
   \file scene.h
   \brief "scene" provide classes for scene definition, without rendering functionality
   
   \ingroup libtascar
   \author Giso Grimm
   \date 2013

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
#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "acousticmodel.h"
#include "audioplugin.h"

namespace TASCAR {

  /** \brief Components relevant for the spatial modelling
   */
  namespace Scene {

    class route_t : public scene_node_base_t {
    public:
      route_t(xmlpp::Element*);
      ~route_t();
      void write_xml();
      std::string get_name() const {return name;};
      bool get_mute() const {return mute;};
      bool get_solo() const {return solo;};
      void set_name(const std::string& s) {name=s;};
      void set_mute(bool b) {mute=b;};
      void set_solo(bool b,uint32_t& anysolo);
      bool is_active(uint32_t anysolo);
      //virtual void prepare(double srate,uint32_t fragsize) {};
      void addmeter(float fs);
      void configure_meter( float tc, TASCAR::levelmeter_t::weight_t w );
      uint32_t metercnt() const { return rmsmeter.size(); };
      void reset_meters();
      const std::vector<float>& readmeter();
      float get_meterval(uint32_t k) const { return meterval[k]; };
    private:
      std::string name;
      bool mute;
      bool solo;
      float meter_tc;
      TASCAR::levelmeter_t::weight_t meter_weight;
    protected:
      std::vector<TASCAR::levelmeter_t*> rmsmeter;
      std::vector<float> meterval;
    };

    class rgb_color_t {
    public:
      rgb_color_t(double r_,double g_,double b_):r(r_),g(g_),b(b_){};
      rgb_color_t(const std::string& webc);
      rgb_color_t():r(0),g(0),b(0){};
      std::string str();
      double r, g, b;
    };

    class sndfile_info_t : public scene_node_base_t {
    public:
      sndfile_info_t(xmlpp::Element* e);
      void write_xml();
      void prepare(double fs, uint32_t fragsize){};
      std::string fname;
      uint32_t firstchannel;
      uint32_t channels;
      uint32_t objectchannel;
      double starttime;
      uint32_t loopcnt;
      double gain;
      std::string parentname;
    };

    class object_t : public dynobject_t, public route_t {
    public:
      object_t(xmlpp::Element*);
      virtual ~object_t() {};
      void write_xml();
      bool isactive(double time) const;
      bool is_active(uint32_t anysolo,double t);
      rgb_color_t color;
      double endtime;
    };

    class sndfile_object_t : public object_t {
    public:
      sndfile_object_t(xmlpp::Element*);
      void write_xml();
      std::vector<sndfile_info_t> sndfiles;
    };

    class mirror_t {
    public:
      mirror_t():c1(0),c2(0){};
      pos_t p;
      double c1;
      double c2;
    };

    class face_object_t : public object_t, public TASCAR::Acousticmodel::reflector_t {
    public:
      face_object_t(xmlpp::Element* xmlsrc);
      virtual ~face_object_t();
      void prepare(double fs, uint32_t fragsize);
      void write_xml();
      void geometry_update(double t);
      void process_active(double t,uint32_t anysolo);
      double width;
      double height;
      std::vector<TASCAR::pos_t> vertices;
    };

    class face_group_t : public object_t {
    public:
      face_group_t(xmlpp::Element* xmlsrc);
      virtual ~face_group_t();
      void prepare(double fs, uint32_t fragsize);
      void write_xml();
      void geometry_update(double t);
      void process_active(double t,uint32_t anysolo);
      std::vector<TASCAR::Acousticmodel::reflector_t*> reflectors;
      double reflectivity;
      double damping;
      std::string importraw;
      bool edgereflection;
      TASCAR::pos_t shoebox;
      TASCAR::pos_t shoeboxwalls;
    };

    class obstacle_group_t : public object_t {
    public:
      obstacle_group_t(xmlpp::Element* xmlsrc);
      virtual ~obstacle_group_t();
      void prepare(double fs, uint32_t fragsize);
      void write_xml();
      void geometry_update(double t);
      void process_active(double t,uint32_t anysolo);
      std::vector<TASCAR::Acousticmodel::obstacle_t*> obstacles;
      double transmission;
      std::string importraw;
    };

    class src_object_t;

    class audio_port_t : public TASCAR::xml_element_t {
    public:
      audio_port_t(xmlpp::Element* e);
      virtual ~audio_port_t();
      void write_xml();
      void set_port_index(uint32_t port_index_);
      uint32_t get_port_index() const { return port_index;};
      void set_portname(const std::string& pn);
      std::string get_portname() const { return portname;};
      std::string get_connect() const { return connect;};
      float get_gain() const { return gain/(2e-5f*caliblevel);};
      float get_gain_db() const { return 20*log10(gain); };
      void set_gain_db( float g );
      void set_gain_lin( float g );
    private:
      std::string portname;
      std::string connect;
      uint32_t port_index;
    public:
      float gain;
      float caliblevel;
    };

    class src_diffuse_t : public sndfile_object_t, public audio_port_t {
    public:
      src_diffuse_t(xmlpp::Element* e);
      virtual ~src_diffuse_t();
      void write_xml();
      void prepare(double fs, uint32_t fragsize);
      void geometry_update(double t);
      void process_active(double t,uint32_t anysolo);
      pos_t size;
      double falloff;
      TASCAR::Acousticmodel::diffuse_source_t* get_source() { return source;};
    private:
      TASCAR::Acousticmodel::diffuse_source_t* source;
    };

    class src_door_t : public object_t, public audio_port_t {
    public:
      src_door_t(xmlpp::Element* e);
      virtual ~src_door_t();
      void write_xml();
      void prepare(double fs, uint32_t fragsize);
      void geometry_update(double t);
      void process_active(double t,uint32_t anysolo);
      double width;
      double height;
      double falloff;
      double distance;
      bool wnd_sqrt;
      double size;
      double maxdist;
      double minlevel;
      uint32_t sincorder;
      TASCAR::Acousticmodel::doorsource_t* get_source() { return source;};
    private:
      TASCAR::Acousticmodel::doorsource_t* source;
    };

    class sound_t : public audio_port_t {
    public:
      sound_t(xmlpp::Element* e,src_object_t* parent_);
    private:
      sound_t(const sound_t& src);
    public:
      virtual ~sound_t();
      void set_parent(src_object_t* parent_);
      void write_xml();
      void geometry_update(double t);
      void process_active(double t,uint32_t anysolo);
      pos_t get_pos_global(double t) const;
      void prepare(double fs, uint32_t fragsize);
      std::string getlabel() const;
      bool isactive(double t);
      bool get_mute() const;
      bool get_solo() const;
      std::string get_port_name() const;
      std::string get_parent_name() const;
      std::string get_name() const;
      void set_name(const std::string& n) {name = n;};
      TASCAR::Acousticmodel::pointsource_t* get_source() { return source;};
      void process_plugins(const TASCAR::transport_t& tp);
      void apply_gain();
    private:
      double fs_;
      double dt_;
      pos_t local_position;
      double chaindist;
    public:
      double size;
      double maxdist;
      double minlevel;
      TASCAR::gainmodel_t gainmodel;
      std::vector<TASCAR::audioplugin_t*> plugins;
    private:
      uint32_t sincorder;
      src_object_t* parent;
      std::string name;
      //bool direct;
    public:
      uint32_t ismmin;
      uint32_t ismmax;
    private:
      // dynamically allocated source type. Allocated in "prepare",
      // type defined in xml_read:
      TASCAR::Acousticmodel::pointsource_t* source; 
      double gain_;
    };

    class src_object_t : public sndfile_object_t {
    public:
      src_object_t(xmlpp::Element* e);
      ~src_object_t();
      void write_xml();
      sound_t* add_sound();
      void prepare(double fs, uint32_t fragsize);
      void geometry_update(double t);
      void process_active(double t,uint32_t anysolo);
      std::vector<sound_t*> sound;
    private:
      int32_t startframe;
    };

    class receivermod_object_t : public object_t, public audio_port_t, public TASCAR::Acousticmodel::receiver_t {
    public:
      receivermod_object_t(xmlpp::Element* e);
      void write_xml();
      void prepare(double fs, uint32_t fragsize);
      void geometry_update(double t);
      void process_active(double t,uint32_t anysolo);
      virtual void postproc(std::vector<wave_t>& output);
    };

    class mask_object_t : public object_t, public TASCAR::Acousticmodel::mask_t {
    public:
      mask_object_t(xmlpp::Element* e);
      void write_xml();
      void prepare(double fs, uint32_t fragsize);
      void geometry_update(double t);
      void process_active(double t,uint32_t anysolo);
      pos_t xmlsize;
      double xmlfalloff;
    };

    class scene_t : public scene_node_base_t {
    public:
      scene_t(xmlpp::Element* e);
      ~scene_t();
      void write_xml();
      src_object_t* add_source();
      std::vector<sound_t*> linearize_sounds();
      void prepare(double fs, uint32_t fragsize);
      std::string description;
      std::string name;
      double c;
      void geometry_update(double t);
      void process_active(double t);
      std::vector<src_object_t*> object_sources;
      std::vector<src_diffuse_t*> diffuse_sources;
      std::vector<src_door_t*> door_sources;
      std::vector<face_object_t*> faces;
      std::vector<face_group_t*> facegroups;
      std::vector<obstacle_group_t*> obstaclegroups;
      std::vector<receivermod_object_t*> receivermod_objects;
      std::vector<mask_object_t*> masks;
      std::vector<object_t*> all_objects;
      std::vector<object_t*> find_object(const std::string& pattern);
      uint32_t mirrororder;
      bool b_0_14;
      double guiscale;
      pos_t guicenter;
      TASCAR::Scene::object_t* guitrackobject;
      uint32_t anysolo;
      std::vector<object_t*> get_objects();
      std::string scene_path;
      void configure_meter( float tc, TASCAR::levelmeter_t::weight_t w );
    private:
      void clean_children();
      scene_t(const scene_t&);
    };

  }

}

std::string jacknamer(const std::string& scenename, const std::string& base);

#endif

/*
 * Local Variables:
 * mode: c++
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * compile-command: "make -C .."
 * End:
 */
