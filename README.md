# JG-Granular
This is a real-time granular fx plugin made with JUCE and gen~ c++ export. 

![JG-Granular.png](Resources/jg-granular.png)

This project is an example of using APVTS (juce::AudioProcessorValueTreeState) to link the exported code with custom JUCE UIs, 
so I will not add any new features to this plugin.

## Installing

Visit the [Releases](https://github.com/szkkng/jg-granular/releases/tag/v1.0) page.

## Building

```
$ git clone https://github.com/szkkng/jg-granular.git --recursive
$ cd jg-granular
$ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
$ cmake --build build --config Release
```

## UI Manual
- value changes: dragging or arrow keys
- fine mode: shift + dragging or shift + arrow keys
- edit mode: 0-9
- undo: [mac] cmd + z, [win] ctrl + z
- redo: [mac] cmd + shift + z, [win] ctrl + shift + z
- reset: double click

## Links
- Demo: [JG-Granular - JUCE × gen~ C++ Export](https://youtu.be/nc4ykXJHpyg)

## References
- [Cycling74/gen-plugin-export](https://github.com/Cycling74/gen-plugin-export)

