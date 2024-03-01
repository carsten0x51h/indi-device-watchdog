/*****************************************************************************
 *
 *  INDI device watchdog  - Monitors the specified INDI devices and optionally
 *  the corresponding Linux devices and tries to keep them connected. Under
 *  certain conditions the device watchdog restarts the respective INDI driver
 *  without restarting the complete INDI server.
 *
 *  Copyright(C) 2024 Carsten Schmitt <c [at] lost-infinity.com>
 *
 *  More info on https://www.lost-infinity.com
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 ****************************************************************************/

#ifndef SOURCE_INDI_DEVICE_WATCHDOG_OPTION_LEVEL_H_
#define SOURCE_INDI_DEVICE_WATCHDOG_OPTION_LEVEL_H_ SOURCE_INDI_DEVICE_WATCHDOG_OPTION_LEVEL_H_

#include <boost/regex.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>

template <unsigned SHORT_NAME>
struct OptionLevelT {
public:
  unsigned n;
  explicit OptionLevelT(unsigned n_ = 0) : n(n_) {}
  OptionLevelT & inc(unsigned by = 1) { n += by; return *this; }
  OptionLevelT & set(unsigned val) { n = val; return *this; }
};


template <unsigned SHORT_NAME>
void validate(boost::any & v, const std::vector<std::string> & values, OptionLevelT<SHORT_NAME> * /*target_type*/, int)
{
  using namespace boost::program_options;
  
  // 
  // Get the current value
  // 
  OptionLevelT<SHORT_NAME> i;
  if (!v.empty())
    i = boost::any_cast<OptionLevelT<SHORT_NAME> >(v);
  
  //
  //  Extract any arguments 
  // 
  const std::string& s = validators::get_single_string(values, true);
  if (s.empty()) {
    v = boost::any(i.inc());
    return;
  }
  
  char short_name = SHORT_NAME;
  
  // multiple 'values's
  if (s == std::string(s.length(), short_name)) {
    v = boost::any(i.inc(s.length() + 1));
    return;
  }
  
  // match number
  boost::regex r("^(\\d+)$");
  
  // Do regex match and convert the interesting part to int.
  boost::smatch what;
  if (regex_match(s, what, r)) {
    v = boost::any(i.set(boost::lexical_cast<unsigned>(s)));
    return;
  } else {
    throw validation_error(validation_error::invalid_option_value, "\"" + s + "\" is not a valid argument.");
  }        
}

template<class T, class charT = char>
class t_level_value : public boost::program_options::typed_value<T, charT> {
public:
  t_level_value(T * store_to) : boost::program_options::typed_value<T, charT>(store_to) {} 
  unsigned min_tokens() const { return 0; }
  unsigned max_tokens() const { return 1; }
};


template<class T>
t_level_value<T>* level_value(T* v) {
  return new t_level_value<T>(v);
};


#endif /* SOURCE_INDI_DEVICE_WATCHDOG_OPTION_LEVEL_H_ */
