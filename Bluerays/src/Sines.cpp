#include "BlueraysSines.hpp"


struct MyModule : Module {
	enum ParamIds {
		PITCH_PARAM,

		OP0LEVEL_PARAM,
		OP0COURSE_PARAM,
		OP0FINE_PARAM,
		
		OP1LEVEL_PARAM,
		OP1COURSE_PARAM,
		OP1FINE_PARAM,

		OP2LEVEL_PARAM,
		OP2COURSE_PARAM,
		OP2FINE_PARAM,

		OP3LEVEL_PARAM,
		OP3COURSE_PARAM,
		OP3FINE_PARAM,

		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,

		OP0LEVEL_INPUT,
		OP0FM_INPUT,

		OP1LEVEL_INPUT,
		OP1FM_INPUT,

		OP2LEVEL_INPUT,
		OP2FM_INPUT,

		OP3LEVEL_INPUT,
		OP3FM_INPUT,

		NUM_INPUTS
	};
	enum OutputIds {
		SINE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	float phase = 0.0;
	float blinkPhase = 0.0;


	float op0level = 0;
	float op0fm = 0;
	float op0fine = 0;
	int op0course = 1;

	float op1level = 0;
	float op1fm = 0;
	float op1fine = 0;
	int op1course = 1;

	float op2level = 0;
	float op2fm = 0;
	float op2fine = 0;
	int op2course = 1;

	int op3course = 1;
	float op3fm = 0;
	float op3level = 0;
	float op3fine = 0.0;

	MyModule() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
	void setParams();

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void MyModule::setParams()
{
	op0course = params[OP0COURSE_PARAM].value;
	op0fine = params[OP0FINE_PARAM].value;
	op0fm = inputs[OP0FM_INPUT].value;
	op0fm = clamp(op0fm, -5.0f, 5.0f);
	op0level = params[OP0LEVEL_PARAM].value;
	op0level += inputs[OP0LEVEL_INPUT].value;
	op0level = clamp(op0level, 0.0, 1.0f);


	op1level = params[OP1LEVEL_PARAM].value;
	op1level += inputs[OP1LEVEL_INPUT].value;
	op1course = params[OP1COURSE_PARAM].value;
	op1fine = params[OP1FINE_PARAM].value;
	op1fm = inputs[OP1FM_INPUT].value;
	op1fm = clamp(op1fm, -5.0f, 5.0f);
	op1level = clamp(op1level, 0.0, 1.0f);

	op2course = params[OP2COURSE_PARAM].value;
	op2fine = params[OP2FINE_PARAM].value;
	op2level = params[OP2LEVEL_PARAM].value;
	op2level += inputs[OP2LEVEL_INPUT].value;
	op2fm = inputs[OP2FM_INPUT].value;
	op2fm = clamp(op2fm, -5.0f, 5.0f);
	op2level = clamp(op2level, 0.0, 1.0f);

	op3course = params[OP3COURSE_PARAM].value;
	op3fine = params[OP3FINE_PARAM].value;
	op3level = params[OP3LEVEL_PARAM].value;
	op3level += inputs[OP3LEVEL_INPUT].value;
	op3fm = inputs[OP3FM_INPUT].value;
	op3level = clamp(op3level, 0.0, 1.0f);
	op3fm = clamp(op3fm, -5.0f, 5.0f);
}

void MyModule::step() {
	// Implement a simple sine oscillator
	float deltaTime = engineGetSampleTime();

	MyModule::setParams();
	// Compute the frequency from the pitch parameter and input
	float pitch = params[PITCH_PARAM].value;
	pitch += inputs[PITCH_INPUT].value;
	pitch = clamp(pitch, -4.0f, 4.0f);
	// The default pitch is C4
	float freq = 261.626f * powf(2.0f, pitch);

	// Accumulate the phase
	phase += freq * deltaTime;
	if (phase >= 1.0f)
		phase -= 1.0f;


	// Compute the sine output
	float sine = op0level * sinf(2.0f * M_PI * (phase + op0fm) * (op0course + op0fine) +
		(op1level * (sinf(2.0f * M_PI * (phase + op1fm) * (op1course + op1fine) + 
			(op2level * (sinf(2.0f * M_PI * (phase + op2fm) * (op2course + op2fine) + (op3level * (sinf(2.0f * M_PI * (phase + op3fm) * (op3course + op3fine)))))))))));

	outputs[SINE_OUTPUT].value = 5.0f * sine;
}


struct MyModuleWidget : ModuleWidget {
	MyModuleWidget(MyModule *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/MyModule.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		float hAdd = 32.3;
		int g = 50;

		addInput(Port::create<PJ301MPort>(Vec(10, box.size.y / 2 - 10 + 125 + 10 + 5), Port::INPUT, module, MyModule::PITCH_INPUT));
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(box.size.x / 2 - 10, box.size.y / 2 - 10 + 125 + 10 + 5), module, MyModule::PITCH_PARAM, -3.0, 3.0, 0.0));

		//operator 0
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(45, 60 + hAdd), module, MyModule::OP3COURSE_PARAM, 1, 48, 0.0));
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(box.size.x / 2 - 12.5, 60 + hAdd), module, MyModule::OP3FINE_PARAM, 0, 1.0, 0.0));
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(box.size.x - 25 - 45, 60 + hAdd), module, MyModule::OP3LEVEL_PARAM, 0, 1.0, 0.0));
		addInput(Port::create<PJ301MPort>(Vec(box.size.x - 25 - 10, 10 + g + hAdd), Port::INPUT, module, MyModule::OP3LEVEL_INPUT));
		addInput(Port::create<PJ301MPort>(Vec(10, 10 + g + hAdd), Port::INPUT, module, MyModule::OP3FM_INPUT));

		//operator 1
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(45, 112.5 + hAdd), module, MyModule::OP2COURSE_PARAM, 1, 48, 0.0));
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(box.size.x / 2 - 12.5, 112.5 + hAdd), module, MyModule::OP2FINE_PARAM, 0, 1.0, 0.0));
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(box.size.x - 25 - 45, 112.5 + hAdd), module, MyModule::OP2LEVEL_PARAM, 0, 1.0, 0.0));
		addInput(Port::create<PJ301MPort>(Vec(box.size.x - 25 - 10, 112.5 + hAdd), Port::INPUT, module, MyModule::OP2LEVEL_INPUT));
		addInput(Port::create<PJ301MPort>(Vec(10, 112.5 + hAdd), Port::INPUT, module, MyModule::OP2FM_INPUT));

		//operator 2
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(45, 167.5 + hAdd), module, MyModule::OP1COURSE_PARAM, 1, 48, 0.0));
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(box.size.x / 2 - 12.5, 167.5 + hAdd), module, MyModule::OP1FINE_PARAM, 0, 1.0, 0.0));
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(box.size.x - 25 - 45, 167.5 + hAdd), module, MyModule::OP1LEVEL_PARAM, 0, 1.0, 0.0));
		addInput(Port::create<PJ301MPort>(Vec(box.size.x - 25 - 10, 167.5 + hAdd), Port::INPUT, module, MyModule::OP1LEVEL_INPUT));
		addInput(Port::create<PJ301MPort>(Vec(10, 167.5 + hAdd), Port::INPUT, module, MyModule::OP1FM_INPUT));

		//operator 3
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(45, 222 + hAdd), module, MyModule::OP0COURSE_PARAM, 1, 48, 0.0));
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(box.size.x / 2 - 12.5, 222 + hAdd), module, MyModule::OP0FINE_PARAM, 0, 1.0, 0.0));
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(box.size.x - 25 - 45, 222 + hAdd), module, MyModule::OP0LEVEL_PARAM, 0, 1.0, 0.0));
		addInput(Port::create<PJ301MPort>(Vec(box.size.x - 25 - 10, 222 + hAdd), Port::INPUT, module, MyModule::OP0LEVEL_INPUT));
		addInput(Port::create<PJ301MPort>(Vec(10, 222 + hAdd), Port::INPUT, module, MyModule::OP0FM_INPUT));




		


		addOutput(Port::create<PJ301MPort>(Vec(box.size.x - 25 - 10, box.size.y / 2 - 10 + 125 + 10 + 5), Port::OUTPUT, module, MyModule::SINE_OUTPUT));

	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelMyModule = Model::create<MyModule, MyModuleWidget>("Bluerays", "Sines", "Sines", OSCILLATOR_TAG);
