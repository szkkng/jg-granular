#include "gen_granular.h"

namespace gen_granular {

/*******************************************************************************************************************
Cycling '74 License for Max-Generated Code for Export
Copyright (c) 2022 Cycling '74
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
	Delay m_del_inR_5;
	Delay m_del_inL_6;
	int __exception;
	int __loopcount;
	int vectorsize;
	t_sample m_grainPos_12;
	t_sample m_interval_11;
	t_sample m_his_index_3;
	t_sample __m_count_14;
	t_sample m_grainSize_13;
	t_sample m_width_10;
	t_sample m_pitch_9;
	t_sample m_mix_8;
	t_sample m_his_gain_1;
	t_sample m_his_mix_2;
	t_sample samplerate;
	t_sample __m_carry_16;
	t_sample m_gain_7;
	// re-initialize all member variables;
	inline void reset(t_param __sr, int __vs) {
		__exception = 0;
		vectorsize = __vs;
		samplerate = __sr;
		m_his_gain_1 = ((int)0);
		m_his_mix_2 = ((int)0);
		m_his_index_3 = ((int)0);
		m_data_param_4.reset("data_param", ((int)6), ((int)100));
		m_del_inR_5.reset("m_del_inR_5", (((int)5) * samplerate));
		m_del_inL_6.reset("m_del_inL_6", (((int)5) * samplerate));
		m_gain_7 = ((int)0);
		m_mix_8 = ((int)50);
		m_pitch_9 = ((int)0);
		m_width_10 = ((int)50);
		m_interval_11 = ((int)100);
		m_grainPos_12 = ((int)100);
		m_grainSize_13 = ((int)100);
		__m_count_14 = 0;
		__m_carry_16 = 0;
		genlib_reset_complete(this);
		
	};
	// the signal processing routine;
	inline int perform(t_sample ** __ins, t_sample ** __outs, int __n) {
		vectorsize = __n;
		const t_sample * __in1 = __ins[0];
		const t_sample * __in2 = __ins[1];
		t_sample * __out1 = __outs[0];
		t_sample * __out2 = __outs[1];
		if (__exception) {
			return __exception;
			
		} else if (( (__in1 == 0) || (__in2 == 0) || (__out1 == 0) || (__out2 == 0) )) {
			__exception = GENLIB_ERR_NULL_BUFFER;
			return __exception;
			
		};
		t_sample count_max_17 = (m_interval_11 * (samplerate * 0.001));
		__loopcount = (__n * GENLIB_LOOPCOUNT_BAIL);
		// the main sample loop;
		while ((__n--)) {
			const t_sample in1 = (*(__in1++));
			const t_sample in2 = (*(__in2++));
			m_his_mix_2 = ((((m_mix_8 * ((t_sample)0.01)) - m_his_mix_2) * ((t_sample)0.001)) + m_his_mix_2);
			m_his_gain_1 = (((dbtoa(m_gain_7) - m_his_gain_1) * ((t_sample)0.001)) + m_his_gain_1);
			m_del_inL_6.write(in1);
			m_del_inR_5.write(in2);
			__m_count_14 = (((int)0) ? 0 : (fixdenorm(__m_count_14 + ((int)1))));
			int carry_15 = 0;
			if ((((int)0) != 0)) {
				__m_count_14 = 0;
				__m_carry_16 = 0;
				
			} else if (((count_max_17 > 0) && (__m_count_14 >= count_max_17))) {
				int wraps_18 = (__m_count_14 / count_max_17);
				__m_carry_16 = (__m_carry_16 + wraps_18);
				__m_count_14 = (__m_count_14 - (wraps_18 * count_max_17));
				carry_15 = 1;
				
			};
			int mCout = __m_count_14;
			if ((mCout == ((int)1))) {
				int data_param_dim = m_data_param_4.dim;
				int data_param_channels = m_data_param_4.channels;
				// samples data_param channel 1;
				int chan_19 = m_his_index_3;
				bool chan_ignore_20 = ((chan_19 < 0) || (chan_19 >= data_param_channels));
				t_sample peek_726 = (chan_ignore_20 ? 0 : m_data_param_4.read(0, chan_19));
				t_sample peek_727 = ((int)0);
				if ((peek_726 == ((int)0))) {
					int channel_trunc_21 = fixnan(floor(m_his_index_3));
					bool chan_ignore_22 = ((channel_trunc_21 < 0) || (channel_trunc_21 >= data_param_channels));
					if ((!chan_ignore_22)) {
						m_data_param_4.write(((int)1), 0, channel_trunc_21);
						
					};
					int channel_trunc_23 = fixnan(floor(m_his_index_3));
					bool chan_ignore_24 = ((channel_trunc_23 < 0) || (channel_trunc_23 >= data_param_channels));
					bool index_ignore_25 = (((int)1) >= data_param_dim);
					if ((!(chan_ignore_24 || index_ignore_25))) {
						m_data_param_4.write((m_grainSize_13 * (samplerate * 0.001)), ((int)1), channel_trunc_23);
						
					};
					t_sample sizeScaled = (m_grainSize_13 * (samplerate * 0.001));
					t_sample widthRand = (((noise() * ((t_sample)0.5)) * m_width_10) * ((t_sample)0.01));
					int channel_trunc_26 = fixnan(floor(m_his_index_3));
					bool chan_ignore_27 = ((channel_trunc_26 < 0) || (channel_trunc_26 >= data_param_channels));
					bool index_ignore_28 = (((int)3) >= data_param_dim);
					if ((!(chan_ignore_27 || index_ignore_28))) {
						m_data_param_4.write((((t_sample)0.5) + widthRand), ((int)3), channel_trunc_26);
						
					};
					int channel_trunc_29 = fixnan(floor(m_his_index_3));
					bool chan_ignore_30 = ((channel_trunc_29 < 0) || (channel_trunc_29 >= data_param_channels));
					bool index_ignore_31 = (((int)4) >= data_param_dim);
					if ((!(chan_ignore_30 || index_ignore_31))) {
						m_data_param_4.write((((t_sample)0.5) - widthRand), ((int)4), channel_trunc_29);
						
					};
					t_sample pitchScaled = (safepow(((int)2), (m_pitch_9 * ((t_sample)0.083333333333333))) - ((int)1));
					int channel_trunc_32 = fixnan(floor(m_his_index_3));
					bool chan_ignore_33 = ((channel_trunc_32 < 0) || (channel_trunc_32 >= data_param_channels));
					bool index_ignore_34 = (((int)5) >= data_param_dim);
					if ((!(chan_ignore_33 || index_ignore_34))) {
						m_data_param_4.write(pitchScaled, ((int)5), channel_trunc_32);
						
					};
					m_his_index_3 = wrap((m_his_index_3 + ((int)1)), ((int)0), ((int)100));
					t_sample posScaled = ((m_grainPos_12 * (samplerate * 0.001)) * fabs(noise()));
					int channel_trunc_35 = fixnan(floor(m_his_index_3));
					bool chan_ignore_36 = ((channel_trunc_35 < 0) || (channel_trunc_35 >= data_param_channels));
					bool index_ignore_37 = (((int)2) >= data_param_dim);
					if ((!(chan_ignore_36 || index_ignore_37))) {
						t_sample maxa_38 = (sizeScaled * pitchScaled);
						m_data_param_4.write((posScaled + ((maxa_38 < ((int)0)) ? ((int)0) : maxa_38)), ((int)2), channel_trunc_35);
						
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
				int chan_39 = i;
				bool chan_ignore_40 = ((chan_39 < 0) || (chan_39 >= data_param_channels));
				t_sample pCout = (chan_ignore_40 ? 0 : m_data_param_4.read(0, chan_39));
				if ((pCout != ((int)0))) {
					bool index_ignore_41 = (((int)1) >= data_param_dim);
					// samples data_param channel 1;
					int chan_42 = i;
					bool chan_ignore_43 = ((chan_42 < 0) || (chan_42 >= data_param_channels));
					t_sample s = ((chan_ignore_43 || index_ignore_41) ? 0 : m_data_param_4.read(((int)1), chan_42));
					if ((pCout < s)) {
						t_sample pCoutScaled = safediv(pCout, s);
						t_sample win = (((t_sample)0.5) * (sin((((t_sample)4.7123889803847) + (pCoutScaled * ((t_sample)6.2831853071796)))) + ((int)1)));
						bool index_ignore_44 = (((int)2) >= data_param_dim);
						// samples data_param channel 1;
						int chan_45 = i;
						bool chan_ignore_46 = ((chan_45 < 0) || (chan_45 >= data_param_channels));
						t_sample peek_728 = ((chan_ignore_46 || index_ignore_44) ? 0 : m_data_param_4.read(((int)2), chan_45));
						t_sample peek_729 = ((int)2);
						bool index_ignore_47 = (((int)5) >= data_param_dim);
						// samples data_param channel 1;
						int chan_48 = i;
						bool chan_ignore_49 = ((chan_48 < 0) || (chan_48 >= data_param_channels));
						t_sample peek_730 = ((chan_ignore_49 || index_ignore_47) ? 0 : m_data_param_4.read(((int)5), chan_48));
						t_sample peek_731 = ((int)5);
						t_sample outputL = (m_del_inL_6.read_linear((peek_728 - (pCout * peek_730))) * win);
						bool index_ignore_50 = (((int)2) >= data_param_dim);
						// samples data_param channel 1;
						int chan_51 = i;
						bool chan_ignore_52 = ((chan_51 < 0) || (chan_51 >= data_param_channels));
						t_sample peek_732 = ((chan_ignore_52 || index_ignore_50) ? 0 : m_data_param_4.read(((int)2), chan_51));
						t_sample peek_733 = ((int)2);
						bool index_ignore_53 = (((int)5) >= data_param_dim);
						// samples data_param channel 1;
						int chan_54 = i;
						bool chan_ignore_55 = ((chan_54 < 0) || (chan_54 >= data_param_channels));
						t_sample peek_734 = ((chan_ignore_55 || index_ignore_53) ? 0 : m_data_param_4.read(((int)5), chan_54));
						t_sample peek_735 = ((int)5);
						t_sample outputR = (m_del_inR_5.read_linear((peek_732 - (pCout * peek_734))) * win);
						bool index_ignore_56 = (((int)3) >= data_param_dim);
						// samples data_param channel 1;
						int chan_57 = i;
						bool chan_ignore_58 = ((chan_57 < 0) || (chan_57 >= data_param_channels));
						t_sample peek_736 = ((chan_ignore_58 || index_ignore_56) ? 0 : m_data_param_4.read(((int)3), chan_57));
						t_sample peek_737 = ((int)3);
						outL = (outL + (outputL * peek_736));
						bool index_ignore_59 = (((int)4) >= data_param_dim);
						// samples data_param channel 1;
						int chan_60 = i;
						bool chan_ignore_61 = ((chan_60 < 0) || (chan_60 >= data_param_channels));
						t_sample peek_738 = ((chan_ignore_61 || index_ignore_59) ? 0 : m_data_param_4.read(((int)4), chan_60));
						t_sample peek_739 = ((int)4);
						outR = (outR + (outputR * peek_738));
						bool chan_ignore_62 = ((i < 0) || (i >= data_param_channels));
						if ((!chan_ignore_62)) {
							m_data_param_4.write((pCout + ((int)1)), 0, i);
							
						};
						totalWin = (totalWin + win);
						
					} else {
						bool chan_ignore_63 = ((i < 0) || (i >= data_param_channels));
						if ((!chan_ignore_63)) {
							m_data_param_4.write(((int)0), 0, i);
							
						};
						
					};
					
				};
				// for loop increment;
				i = (i + ((int)1));
				
			};
			t_sample normalise = (safepow(safediv(((int)1), ((totalWin < ((int)1)) ? ((int)1) : totalWin)), ((t_sample)0.3)) * m_his_gain_1);
			t_sample mul_754 = (outL * normalise);
			t_sample mix_753 = (in1 + (m_his_mix_2 * (mul_754 - in1)));
			t_sample mul_756 = (outR * normalise);
			t_sample mix_755 = (in2 + (m_his_mix_2 * (mul_756 - in2)));
			t_sample out1 = mix_753;
			t_sample out2 = mix_755;
			m_del_inR_5.step();
			m_del_inL_6.step();
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
		m_gain_7 = (_value < -36 ? -36 : (_value > 12 ? 12 : _value));
	};
	inline void set_mix(t_param _value) {
		m_mix_8 = (_value < 0 ? 0 : (_value > 100 ? 100 : _value));
	};
	inline void set_pitch(t_param _value) {
		m_pitch_9 = (_value < -12 ? -12 : (_value > 12 ? 12 : _value));
	};
	inline void set_width(t_param _value) {
		m_width_10 = (_value < 0 ? 0 : (_value > 100 ? 100 : _value));
	};
	inline void set_interval(t_param _value) {
		m_interval_11 = (_value < 10 ? 10 : (_value > 500 ? 500 : _value));
	};
	inline void set_grainPos(t_param _value) {
		m_grainPos_12 = (_value < 10 ? 10 : (_value > 500 ? 500 : _value));
	};
	inline void set_grainSize(t_param _value) {
		m_grainSize_13 = (_value < 10 ? 10 : (_value > 500 ? 500 : _value));
	};
	
} State;


///
///	Configuration for the genlib API
///

/// Number of signal inputs and outputs

int gen_kernel_numins = 2;
int gen_kernel_numouts = 2;

int num_inputs() { return gen_kernel_numins; }
int num_outputs() { return gen_kernel_numouts; }
int num_params() { return 8; }

/// Assistive lables for the signal inputs and outputs

const char *gen_kernel_innames[] = { "in1", "in2" };
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
		
		case 1: *value = self->m_gain_7; break;
		case 2: *value = self->m_grainPos_12; break;
		case 3: *value = self->m_grainSize_13; break;
		case 4: *value = self->m_interval_11; break;
		case 5: *value = self->m_mix_8; break;
		case 6: *value = self->m_pitch_9; break;
		case 7: *value = self->m_width_10; break;
		
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
	// initialize parameter 1 ("m_gain_7")
	pi = self->__commonstate.params + 1;
	pi->name = "gain";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_gain_7;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = -36;
	pi->outputmax = 12;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 2 ("m_grainPos_12")
	pi = self->__commonstate.params + 2;
	pi->name = "grainPos";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_grainPos_12;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 10;
	pi->outputmax = 500;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 3 ("m_grainSize_13")
	pi = self->__commonstate.params + 3;
	pi->name = "grainSize";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_grainSize_13;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 10;
	pi->outputmax = 500;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 4 ("m_interval_11")
	pi = self->__commonstate.params + 4;
	pi->name = "interval";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_interval_11;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 10;
	pi->outputmax = 500;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 5 ("m_mix_8")
	pi = self->__commonstate.params + 5;
	pi->name = "mix";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_mix_8;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 100;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 6 ("m_pitch_9")
	pi = self->__commonstate.params + 6;
	pi->name = "pitch";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_pitch_9;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = -12;
	pi->outputmax = 12;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 7 ("m_width_10")
	pi = self->__commonstate.params + 7;
	pi->name = "width";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_width_10;
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
