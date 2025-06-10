#ifndef __LTGRAPH__
#define __LTGRAPH__

#include <string>
#include <vector>

namespace LFEA {
    enum SimulationType { None, Transient, AnalysisAC, SweepDC, Noise, TransferDC, OperatingPoint, TransientFR };

    class LTgraph {
    public:
        struct Waveform {
            Waveform() = default;
            Waveform(std::string _name){
                name = _name;
                values = std::vector<std::vector<double>>(1, std::vector<double>());
            }

            inline const std::string AxisLabel(int i) { return name + " [" + units[i+1] + "]"; }

            int paramCount = -1;
            std::string name;
            std::vector<std::string> units;
            std::vector<std::vector<double>> values;
        };

        LTgraph(std::string filename);

        inline const std::string AxisLabel() { return name + " [" + unit + "]"; }


        SimulationType simType;
        // double xMin, xMax, xStep;
        std::string name;
        std::string unit;
        std::vector<double> values;

        std::vector<Waveform> waves;
    };
}

#endif