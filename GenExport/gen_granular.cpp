#include "gen_granular.h"

namespace gen_granular {

/*******************************************************************************************************************
Cycling '74 License for Max-Generated Code for Export
Copyright (c) 2016 Cycling '74
The code that Max generates automatically and that end users are capable of exporting and using, and any
  associated documentation files (the “Software”) is a work of authorship for which Cycling '74 is the author
  and owner for copyright purposes.  A license is hereby granted, free of charge, to any person obtaining a
  copy of the Software (“Licensee”) to use, copy, modify, merge, publish, and distribute copies of the Software,
  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The Software is licensed to Licensee only for non-commercial use. Users who wish to make commercial use of the
  Software must contact the copyright owner to determine if a license for commercial use is available, and the
  terms and conditions for same, which may include fees or royalties. For commercial use, please send inquiries
  to licensing (at) cycling74.com.  The determination of whether a use is commercial use or non-commercial use is based
  upon the use, not the user. The Software may be used by individuals, institutions, governments, corporations, or
  other business whether for-profit or non-profit so long as the use itself is not a commercialization of the
  materials or a use that generates or is intended to generate income, revenue, sales or profit.
The above copyright notice and this license shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
  THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*******************************************************************************************************************/

// global noise generator
Noise noise;
static const int GENLIB_LOOPCOUNT_BAIL = 100000;


// The State struct contains all the state and procedures for the gendsp kernel
typedef struct State {
	CommonState __commonstate;
	Data m_data_param_4;
	Delay m_del_in_5;
	int __exception;
	int __loopcount;
	int vectorsize;
	t_sample m_grainPos_11;
	t_sample m_interval_10;
	t_sample m_his_mix_2;
	t_sample __m_count_13;
	t_sample m_grainSize_12;
	t_sample m_width_9;
	t_sample m_pitch_8;
	t_sample m_mix_7;
	t_sample m_his_index_3;
	t_sample m_his_gain_1;
	t_sample samplerate;
	t_sample __m_carry_15;
	t_sample m_gain_6;
	// re-initialize all member variables;
	inline void reset(t_param __sr, int __vs) {
		__exception = 0;
		vectorsize = __vs;
		samplerate = __sr;
		m_his_gain_1 = ((int)0);
		m_his_mix_2 = ((int)0);
		m_his_index_3 = ((int)0);
		m_data_param_4.reset("data_param", ((int)6), ((int)100));
		m_del_in_5.reset("m_del_in_5", (((int)5) * samplerate));
		m_gain_6 = ((int)0);
		m_mix_7 = ((int)50);
		m_pitch_8 = ((int)0);
		m_width_9 = ((int)50);
		m_interval_10 = ((int)100);
		m_grainPos_11 = ((int)100);
		m_grainSize_12 = ((int)100);
		__m_count_13 = 0;
		__m_carry_15 = 0;
		genlib_reset_complete(this);
		
	};
	// the signal processing routine;
	inline int perform(t_sample ** __ins, t_sample ** __outs, int __n) {
		vectorsize = __n;
		const t_sample * __in1 = __ins[0];
		t_sample * __out1 = __outs[0];
		t_sample * __out2 = __outs[1];
		if (__exception) {
			return __exception;
			
		} else if (( (__in1 == 0) || (__out1 == 0) || (__out2 == 0) )) {
			__exception = GENLIB_ERR_NULL_BUFFER;
			return __exception;
			
		};
		t_sample count_max_16 = (m_interval_10 * (samplerate * 0.001));
		__loopcount = (__n * GENLIB_LOOPCOUNT_BAIL);
		// the main sample loop;
		while ((__n--)) {
			const t_sample in1 = (*(__in1++));
			m_his_mix_2 = ((((m_mix_7 * ((t_sample)0.01)) - m_his_mix_2) * ((t_sample)0.001)) + m_his_mix_2);
			m_his_gain_1 = (((dbtoa(m_gain_6) - m_his_gain_1) * ((t_sample)0.001)) + m_his_gain_1);
			m_del_in_5.write(in1);
			__m_count_13 = (((int)0) ? 0 : (fixdenorm(__m_count_13 + ((int)1))));
			int carry_14 = 0;
			if ((((int)0) != 0)) {
				__m_count_13 = 0;
				__m_carry_15 = 0;
				
			} else if (((count_max_16 > 0) && (__m_count_13 >= count_max_16))) {
				int wraps_17 = (__m_count_13 / count_max_16);
				__m_carry_15 = (__m_carry_15 + wraps_17);
				__m_count_13 = (__m_count_13 - (wraps_17 * count_max_16));
				carry_14 = 1;
				
			};
			int mCout = __m_count_13;
			if ((mCout == ((int)1))) {
				int data_param_dim = m_data_param_4.dim;
				int data_param_channels = m_data_param_4.channels;
				// samples data_param channel 1;
				int chan_18 = m_his_index_3;
				bool chan_ignore_19 = ((chan_18 < 0) || (chan_18 >= data_param_channels));
				t_sample peek_11640 = (chan_ignore_19 ? 0 : m_data_param_4.read(0, chan_18));
				t_sample peek_11641 = ((int)0);
				if ((peek_11640 == ((int)0))) {
					int channel_trunc_20 = fixnan(floor(m_his_index_3));
					bool chan_ignore_21 = ((channel_trunc_20 < 0) || (channel_trunc_20 >= data_param_channels));
					if ((!chan_ignore_21)) {
						m_data_param_4.write(((int)1), 0, channel_trunc_20);
						
					};
					int channel_trunc_22 = fixnan(floor(m_his_index_3));
					bool chan_ignore_23 = ((channel_trunc_22 < 0) || (channel_trunc_22 >= data_param_channels));
					bool index_ignore_24 = (((int)1) >= data_param_dim);
					if ((!(chan_ignore_23 || index_ignore_24))) {
						m_data_param_4.write((m_grainSize_12 * (samplerate * 0.001)), ((int)1), channel_trunc_22);
						
					};
					t_sample sizeScaled = (m_grainSize_12 * (samplerate * 0.001));
					t_sample widthRand = (((noise() * ((t_sample)0.5)) * m_width_9) * ((t_sample)0.01));
					int channel_trunc_25 = fixnan(floor(m_his_index_3));
					bool chan_ignore_26 = ((channel_trunc_25 < 0) || (channel_trunc_25 >= data_param_channels));
					bool index_ignore_27 = (((int)3) >= data_param_dim);
					if ((!(chan_ignore_26 || index_ignore_27))) {
						m_data_param_4.write((((t_sample)0.5) + widthRand), ((int)3), channel_trunc_25);
						
					};
					int channel_trunc_28 = fixnan(floor(m_his_index_3));
					bool chan_ignore_29 = ((channel_trunc_28 < 0) || (channel_trunc_28 >= data_param_channels));
					bool index_ignore_30 = (((int)4) >= data_param_dim);
					if ((!(chan_ignore_29 || index_ignore_30))) {
						m_data_param_4.write((((t_sample)0.5) - widthRand), ((int)4), channel_trunc_28);
						
					};
					t_sample pitchScaled = (safepow(((int)2), (m_pitch_8 * ((t_sample)0.083333333333333))) - ((int)1));
					int channel_trunc_31 = fixnan(floor(m_his_index_3));
					bool chan_ignore_32 = ((channel_trunc_31 < 0) || (channel_trunc_31 >= data_param_channels));
					bool index_ignore_33 = (((int)5) >= data_param_dim);
					if ((!(chan_ignore_32 || index_ignore_33))) {
						m_data_param_4.write(pitchScaled, ((int)5), channel_trunc_31);
						
					};
					m_his_index_3 = wrap((m_his_index_3 + ((int)1)), ((int)0), ((int)100));
					t_sample posScaled = ((m_grainPos_11 * (samplerate * 0.001)) * fabs(noise()));
					int channel_trunc_34 = fixnan(floor(m_his_index_3));
					bool chan_ignore_35 = ((channel_trunc_34 < 0) || (channel_trunc_34 >= data_param_channels));
					bool index_ignore_36 = (((int)2) >= data_param_dim);
					if ((!(chan_ignore_35 || index_ignore_36))) {
						t_sample maxa_37 = (sizeScaled * pitchScaled);
						m_data_param_4.write((posScaled + ((maxa_37 < ((int)0)) ? ((int)0) : maxa_37)), ((int)2), channel_trunc_34);
						
					};
					
				};
				
			};
			t_sample outL = ((int)0);
			t_sample outR = ((int)0);
			t_sample totalWin = ((int)0);
			// for loop initializer;
			int i = ((int)0);
			// for loop condition;
			// abort processing if an infinite loop is suspected;
			if (((__loopcount--) <= 0)) {
				__exception = GENLIB_ERR_LOOP_OVERFLOW;
				break ;
				
			};
			while ((i < ((int)100))) {
				// abort processing if an infinite loop is suspected;
				if (((__loopcount--) <= 0)) {
					__exception = GENLIB_ERR_LOOP_OVERFLOW;
					break ;
					
				};
				int data_param_dim = m_data_param_4.dim;
				int data_param_channels = m_data_param_4.channels;
				// samples data_param channel 1;
				int chan_38 = i;
				bool chan_ignore_39 = ((chan_38 < 0) || (chan_38 >= data_param_channels));
				t_sample pCout = (chan_ignore_39 ? 0 : m_data_param_4.read(0, chan_38));
				if ((pCout != ((int)0))) {
					bool index_ignore_40 = (((int)1) >= data_param_dim);
					// samples data_param channel 1;
					int chan_41 = i;
					bool chan_ignore_42 = ((chan_41 < 0) || (chan_41 >= data_param_channels));
					t_sample s = ((chan_ignore_42 || index_ignore_40) ? 0 : m_data_param_4.read(((int)1), chan_41));
					if ((pCout < s)) {
						t_sample pCoutScaled = safediv(pCout, s);
						t_sample win = (((t_sample)0.5) * (sin((((t_sample)4.7123889803847) + (pCoutScaled * ((t_sample)6.2831853071796)))) + ((int)1)));
						bool index_ignore_43 = (((int)2) >= data_param_dim);
						// samples data_param channel 1;
						int chan_44 = i;
						bool chan_ignore_45 = ((chan_44 < 0) || (chan_44 >= data_param_channels));
						t_sample peek_11642 = ((chan_ignore_45 || index_ignore_43) ? 0 : m_data_param_4.read(((int)2), chan_44));
						t_sample peek_11643 = ((int)2);
						bool index_ignore_46 = (((int)5) >= data_param_dim);
						// samples data_param channel 1;
						int chan_47 = i;
						bool chan_ignore_48 = ((chan_47 < 0) || (chan_47 >= data_param_channels));
						t_sample peek_11644 = ((chan_ignore_48 || index_ignore_46) ? 0 : m_data_param_4.read(((int)5), chan_47));
						t_sample peek_11645 = ((int)5);
						t_sample output = (m_del_in_5.read_linear((peek_11642 - (pCout * peek_11644))) * win);
						bool index_ignore_49 = (((int)3) >= data_param_dim);
						// samples data_param channel 1;
						int chan_50 = i;
						bool chan_ignore_51 = ((chan_50 < 0) || (chan_50 >= data_param_channels));
						t_sample peek_11646 = ((chan_ignore_51 || index_ignore_49) ? 0 : m_data_param_4.read(((int)3), chan_50));
						t_sample peek_11647 = ((int)3);
						outL = (outL + (output * peek_11646));
						bool index_ignore_52 = (((int)4) >= data_param_dim);
						// samples data_param channel 1;
						int chan_53 = i;
						bool chan_ignore_54 = ((chan_53 < 0) || (chan_53 >= data_param_channels));
						t_sample peek_11648 = ((chan_ignore_54 || index_ignore_52) ? 0 : m_data_param_4.read(((int)4), chan_53));
						t_sample peek_11649 = ((int)4);
						outR = (outR + (output * peek_11648));
						bool chan_ignore_55 = ((i < 0) || (i >= data_param_channels));
						if ((!chan_ignore_55)) {
							m_data_param_4.write((pCout + ((int)1)), 0, i);
							
						};
						totalWin = (totalWin + win);
						
					} else {
						bool chan_ignore_56 = ((i < 0) || (i >= data_param_channels));
						if ((!chan_ignore_56)) {
							m_data_param_4.write(((int)0), 0, i);
							
						};
						
					};
					
				};
				// for loop increment;
				i = (i + ((int)1));
				
			};
			t_sample normalise = (safepow(safediv(((int)1), ((totalWin < ((int)1)) ? ((int)1) : totalWin)), ((t_sample)0.3)) * m_his_gain_1);
			t_sample mul_11657 = (outL * normalise);
			t_sample mix_11656 = (in1 + (m_his_mix_2 * (mul_11657 - in1)));
			t_sample mul_11659 = (outR * normalise);
			t_sample mix_11658 = (in1 + (m_his_mix_2 * (mul_11659 - in1)));
			t_sample out1 = mix_11656;
			t_sample out2 = mix_11658;
			m_del_in_5.step();
			// assign results to output buffer;
			(*(__out1++)) = out1;
			(*(__out2++)) = out2;
			
		};
		return __exception;
		
	};
	inline void set_data_param(void * _value) {
		m_data_param_4.setbuffer(_value);
	};
	inline void set_gain(t_param _value) {
		m_gain_6 = (_value < -36 ? -36 : (_value > 12 ? 12 : _value));
	};
	inline void set_mix(t_param _value) {
		m_mix_7 = (_value < 0 ? 0 : (_value > 100 ? 100 : _value));
	};
	inline void set_pitch(t_param _value) {
		m_pitch_8 = (_value < -12 ? -12 : (_value > 12 ? 12 : _value));
	};
	inline void set_width(t_param _value) {
		m_width_9 = (_value < 0 ? 0 : (_value > 100 ? 100 : _value));
	};
	inline void set_interval(t_param _value) {
		m_interval_10 = (_value < 10 ? 10 : (_value > 500 ? 500 : _value));
	};
	inline void set_grainPos(t_param _value) {
		m_grainPos_11 = (_value < 10 ? 10 : (_value > 500 ? 500 : _value));
	};
	inline void set_grainSize(t_param _value) {
		m_grainSize_12 = (_value < 10 ? 10 : (_value > 500 ? 500 : _value));
	};
	
} State;


///
///	Configuration for the genlib API
///

/// Number of signal inputs and outputs

int gen_kernel_numins = 1;
int gen_kernel_numouts = 2;

int num_inputs() { return gen_kernel_numins; }
int num_outputs() { return gen_kernel_numouts; }
int num_params() { return 8; }

/// Assistive lables for the signal inputs and outputs

const char *gen_kernel_innames[] = { "in1" };
const char *gen_kernel_outnames[] = { "out1", "out2" };

/// Invoke the signal process of a State object

int perform(CommonState *cself, t_sample **ins, long numins, t_sample **outs, long numouts, long n) {
	State* self = (State *)cself;
	return self->perform(ins, outs, n);
}

/// Reset all parameters and stateful operators of a State object

void reset(CommonState *cself) {
	State* self = (State *)cself;
	self->reset(cself->sr, cself->vs);
}

/// Set a parameter of a State object

void setparameter(CommonState *cself, long index, t_param value, void *ref) {
	State *self = (State *)cself;
	switch (index) {
		case 0: self->set_data_param(ref); break;
		case 1: self->set_gain(value); break;
		case 2: self->set_grainPos(value); break;
		case 3: self->set_grainSize(value); break;
		case 4: self->set_interval(value); break;
		case 5: self->set_mix(value); break;
		case 6: self->set_pitch(value); break;
		case 7: self->set_width(value); break;
		
		default: break;
	}
}

/// Get the value of a parameter of a State object

void getparameter(CommonState *cself, long index, t_param *value) {
	State *self = (State *)cself;
	switch (index) {
		
		case 1: *value = self->m_gain_6; break;
		case 2: *value = self->m_grainPos_11; break;
		case 3: *value = self->m_grainSize_12; break;
		case 4: *value = self->m_interval_10; break;
		case 5: *value = self->m_mix_7; break;
		case 6: *value = self->m_pitch_8; break;
		case 7: *value = self->m_width_9; break;
		
		default: break;
	}
}

/// Get the name of a parameter of a State object

const char *getparametername(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].name;
	}
	return 0;
}

/// Get the minimum value of a parameter of a State object

t_param getparametermin(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].outputmin;
	}
	return 0;
}

/// Get the maximum value of a parameter of a State object

t_param getparametermax(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].outputmax;
	}
	return 0;
}

/// Get parameter of a State object has a minimum and maximum value

char getparameterhasminmax(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].hasminmax;
	}
	return 0;
}

/// Get the units of a parameter of a State object

const char *getparameterunits(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].units;
	}
	return 0;
}

/// Get the size of the state of all parameters of a State object

size_t getstatesize(CommonState *cself) {
	return genlib_getstatesize(cself, &getparameter);
}

/// Get the state of all parameters of a State object

short getstate(CommonState *cself, char *state) {
	return genlib_getstate(cself, state, &getparameter);
}

/// set the state of all parameters of a State object

short setstate(CommonState *cself, const char *state) {
	return genlib_setstate(cself, state, &setparameter);
}

/// Allocate and configure a new State object and it's internal CommonState:

void *create(t_param sr, long vs) {
	State *self = new State;
	self->reset(sr, vs);
	ParamInfo *pi;
	self->__commonstate.inputnames = gen_kernel_innames;
	self->__commonstate.outputnames = gen_kernel_outnames;
	self->__commonstate.numins = gen_kernel_numins;
	self->__commonstate.numouts = gen_kernel_numouts;
	self->__commonstate.sr = sr;
	self->__commonstate.vs = vs;
	self->__commonstate.params = (ParamInfo *)genlib_sysmem_newptr(8 * sizeof(ParamInfo));
	self->__commonstate.numparams = 8;
	// initialize parameter 0 ("m_data_param_4")
	pi = self->__commonstate.params + 0;
	pi->name = "data_param";
	pi->paramtype = GENLIB_PARAMTYPE_SYM;
	pi->defaultvalue = 0.;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = false;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 1 ("m_gain_6")
	pi = self->__commonstate.params + 1;
	pi->name = "gain";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_gain_6;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = -36;
	pi->outputmax = 12;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 2 ("m_grainPos_11")
	pi = self->__commonstate.params + 2;
	pi->name = "grainPos";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_grainPos_11;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 10;
	pi->outputmax = 500;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 3 ("m_grainSize_12")
	pi = self->__commonstate.params + 3;
	pi->name = "grainSize";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_grainSize_12;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 10;
	pi->outputmax = 500;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 4 ("m_interval_10")
	pi = self->__commonstate.params + 4;
	pi->name = "interval";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_interval_10;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 10;
	pi->outputmax = 500;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 5 ("m_mix_7")
	pi = self->__commonstate.params + 5;
	pi->name = "mix";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_mix_7;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 100;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 6 ("m_pitch_8")
	pi = self->__commonstate.params + 6;
	pi->name = "pitch";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_pitch_8;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = -12;
	pi->outputmax = 12;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 7 ("m_width_9")
	pi = self->__commonstate.params + 7;
	pi->name = "width";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_width_9;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 100;
	pi->exp = 0;
	pi->units = "";		// no units defined
	
	return self;
}

/// Release all resources and memory used by a State object:

void destroy(CommonState *cself) {
	State *self = (State *)cself;
	genlib_sysmem_freeptr(cself->params);
		
	delete self;
}


} // gen_granular::
