(function main() {

    var bundle;

    bundle = {};
    bundle.classMap = {};

    bundle.classMap.match_any = {
        name: "match-any",
        bandwidth: 5000,
        cos: 0,
        flow: {
            inputRate: 0,
            bandwidth: 0
        },
        match: {
        }
    };

    bundle.classMap.c31 = {
        name: "c31",
        bandwidth: 5000,
        cos: 1,
        flow: {
            inputRate: 0,
            bandwidth: 0
        },
        match: {
            vlan: 1005,
            cos: 0
        }
    };

    bundle.classMap.c32 = {
        name: "c32",
        bandwidth: 1000,
        cos: 4,
        flow: {
            inputRate: 0,
            bandwidth: 0
        },
        match: {
            vlan: 43,
            cos: 4
        }
    };

    bundle.classMap.c33 = {
        name: "c33",
        bandwidth: 1000,
        cos: 4,
        flow: {
            inputRate: 0,
            bandwidth: 0
        },
        match: {
            vlan: 4001,
            cos: 4
        }
    };
    bundle.bandwidth = 5000;

    function updateMeterPerCos(bundle, cos /*, bundleRate*/) {
        var i, name, classMap, newRate, target = {},
            bundleRate = bundle.remainBandwidth;

        target.length = 0;
        target.totalInputRate = 0;
        target.classMap = {};

        for (name in bundle.classMap) {

            if (!bundle.classMap.hasOwnProperty(name)) {
                continue;
            }

            classMap = bundle.classMap[name];

            if (classMap.cos === cos) {
                target.classMap[name] = classMap;

                if (classMap.bandwidth < classMap.flow.inputRate) {
                    classMap.flow.inputRate = classMap.bandwidth;
                }

                target.totalInputRate += classMap.flow.inputRate;
                target.length += 1;
            }
        }

        for (name in target.classMap) {

            if (!target.classMap.hasOwnProperty(name)) {
                continue;
            }

            classMap = target.classMap[name];

            if (target.totalInputRate <= bundleRate ||
                target.totalInputRate === 0 ||
                classMap.flow.bandwidth === 0) {
                newRate = classMap.bandwidth;
            }
            else {
                newRate = classMap.flow.inputRate * bundleRate / target.totalInputRate;
            }

            if (classMap.bandwidth < newRate) {
                newRate = classMap.bandwidth;
            }

            classMap.flow.bandwidth = newRate;
        }

        bundle.remainBandwidth = bundleRate - target.totalInputRate;
        if (bundle.remainBandwidth < 0) {
            bundle.remainBandwidth = 0;
        }
    }

/*
    function getRatePerCos(bundle) {
        var ratePerCos = [],
            cos, name, classMap;

        for (cos = 0; cos <= 8; cos += 1)
            ratePerCos[cos] = 0;

        for (name in bundle.classMap) {
            if (!bundle.classMap.hasOwnProperty(name)) {
                continue;
            }

            classMap = bundle.classMap[ name ];

            ratePerCos[ classMap.cos ] += classMap.flow.inputRate;
        }

        return ratePerCos;
    }
*/
/*
    function getRatePerLevel(ratePerCos) {
        var ratePerLevel = [],
            cos, i;

        for (cos = 0; cos <= 8; cos += 1)
            ratePerLevel[cos] = 0;

        for (cos = 0; cos <= 8; cos += 1) {
            for (i = cos; i <= 8; i += 1) {
                ratePerLevel[cos] += ratePerCos[i];
            }
        }

        return ratePerLevel;
    }
*/
    function run(runObj) {
        var bundle = runObj.bundle,
            cos /*, currentTotalRatePerCos, currentTotalRatePerLevel, bundleRate */,
            sortBundle = [];

        function defaultSetup(bundle) {
            var i, name, classMap, newRate, target = {};

            for (name in bundle.classMap) {
                if (!bundle.classMap.hasOwnProperty(name)) {
                    continue;
                }

                classMap = bundle.classMap[ name ];

                classMap.flow.bandwidth = classMap.bandwidth;
                classMap.flow.inputRate = 0;
            }
        }

        function createSortBundle(bundle) {
            var name, classMap,
                sortBundle = [],
                i;

            for (name in bundle.classMap) {
                if (!bundle.classMap.hasOwnProperty(name)) {
                    continue;
                }

                classMap = bundle.classMap[ name ];

                if (!sortBundle[classMap.cos]) {
                    sortBundle[classMap.cos] = {
                        cos: classMap.cos,
                        classMap: {},
                        length: 0,
                        totalInputRate: 0
                    };
                }

                sortBundle[classMap.cos].classMap[name] = classMap;
                sortBundle[classMap.cos].length += 1;

                if (classMap.bandwidth < classMap.flow.inputRate) {
                    classMap.flow.inputRate = classMap.bandwidth;
                }

                sortBundle[classMap.cos].totalInputRate += classMap.flow.inputRate;
            }

            sortBundle.sort(function(a, b){
                return b.cos - a.cos;
            });

            for (i=0; i < sortBundle.length; i += 1) {
                if (typeof sortBundle[i] === 'undefined') {
                    sortBundle.length = i;
                    break;
                }
            }

            return sortBundle;
        }

        defaultSetup(bundle);
        if (runObj.setup) {
            runObj.setup(bundle);
        }

        bundle.remainBandwidth = bundle.bandwidth;

/*
        currentTotalRatePerCos = getRatePerCos(bundle);
        currentTotalRatePerLevel = getRatePerLevel(currentTotalRatePerCos);
*/
/*        for (cos = 0; cos <= 7; cos += 1) {*/
        for (cos = 7; cos >= 0; cos -= 1) {
/*
            if (currentTotalRatePerLevel[cos+1] < bundle.bandwidth) {
                bundleRate = bundle.bandwidth - currentTotalRatePerLevel[cos+1];
            }
            else {
                bundleRate = 0;
            }
*/
            updateMeterPerCos(bundle, cos /*, bundleRate*/);
        }

        updateMeterPerCos(bundle, 8 /*, bundleRate*/);

        if (runObj.complete) {
            runObj.complete(bundle);
        }
    }

    function AssertException(message) {
        this.message = message;
    }

    AssertException.prototype.toString = function () {
        return 'AssertException: ' + this.message;
    }

    function assert(exp, message) {
        if (!exp) {
            throw new AssertException(message);
        }
    }

    run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.c32.flow.inputRate = 5000;
            bundle.classMap.c33.flow.inputRate = 5000;
        },
        complete: function (bundle) {
            assert(bundle.classMap.c31.flow.bandwidth.toFixed(0) == 5000);
            assert(bundle.classMap.c32.flow.bandwidth.toFixed(0) == 1000);
            assert(bundle.classMap.c33.flow.bandwidth.toFixed(0) == 1000);
        }
    });

    run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.c32.flow.inputRate = 1000;
            bundle.classMap.c33.flow.inputRate = 5000;
        },
        complete: function (bundle) {
            assert(bundle.classMap.c31.flow.bandwidth.toFixed(0) == 5000);

            // 1000 -> 1000, 1/2 * 5000 = 1000 | bandwidth
            assert(bundle.classMap.c32.flow.bandwidth.toFixed(0) == 1000);

            // 5000 -> 1000, 1/2 * 5000 = 1000 | bandwidth
            assert(bundle.classMap.c33.flow.bandwidth.toFixed(0) == 1000);
        }
    });

    run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.c32.flow.inputRate = 2000;
            bundle.classMap.c33.flow.inputRate = 4000;
        },
        complete: function (bundle) {
            assert(bundle.classMap.c31.flow.bandwidth.toFixed(0) == 5000);

            // 2000 -> 1000, 1/2 * 5000 = 1000 | bandwidth
            assert(bundle.classMap.c32.flow.bandwidth.toFixed(0) == 1000);

            // 4000 -> 1000, 1/2 * 5000 = 1000 | bandwidth
            assert(bundle.classMap.c33.flow.bandwidth.toFixed(0) == 1000);
        }
    });

    run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.c32.flow.inputRate = 500;
            bundle.classMap.c33.flow.inputRate = 1000;
        },
        complete: function (bundle) {
            assert(bundle.classMap.c31.flow.bandwidth.toFixed(0) == 5000);

            // 1/3 * 5000 = 1666 | bandwidth
            assert(bundle.classMap.c32.flow.bandwidth.toFixed(0) == 1000);

            // 2/3 * 5000 = 3333 | bandwidth
            assert(bundle.classMap.c33.flow.bandwidth.toFixed(0) == 1000);
        }
    });

    run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.c32.flow.inputRate = 500;
            bundle.classMap.c33.flow.inputRate = 500;
        },
        complete: function (bundle) {
            assert(bundle.classMap.c31.flow.bandwidth.toFixed(0) == 5000);

            // 1/2 * 5000 = 2500 | bandwidth
            assert(bundle.classMap.c32.flow.bandwidth.toFixed(0) == 1000);

            // 1/2 * 5000 = 2500 | bandwidth
            assert(bundle.classMap.c33.flow.bandwidth.toFixed(0) == 1000);
        }
    });

    run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.c31.flow.inputRate = 3000;
            bundle.classMap.c32.flow.inputRate = 1000;
            bundle.classMap.c33.flow.inputRate = 1000;
        },
        complete: function (bundle) {
            assert(bundle.classMap.c31.flow.bandwidth.toFixed(0) == 5000);
            assert(bundle.classMap.c32.flow.bandwidth.toFixed(0) == 1000);
            assert(bundle.classMap.c33.flow.bandwidth.toFixed(0) == 1000);
        }
    });

    run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.c31.flow.inputRate = 5000;
            bundle.classMap.c32.flow.inputRate = 1000;
            bundle.classMap.c33.flow.inputRate = 1000;
        },
        complete: function (bundle) {
            assert(bundle.classMap.c31.flow.bandwidth.toFixed(0) == 3000);
            assert(bundle.classMap.c32.flow.bandwidth.toFixed(0) == 1000);
            assert(bundle.classMap.c33.flow.bandwidth.toFixed(0) == 1000);
        }
    });

    run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.c31.flow.inputRate = 5000;
            bundle.classMap.c32.flow.inputRate =  500;
            bundle.classMap.c33.flow.inputRate = 1000;
        },
        complete: function (bundle) {
            assert(bundle.classMap.c31.flow.bandwidth.toFixed(0) == 3500);

            // under bandwidth
            assert(bundle.classMap.c32.flow.bandwidth.toFixed(0) == 1000);
            assert(bundle.classMap.c33.flow.bandwidth.toFixed(0) == 1000);
        }
    });

    run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.match_any.flow.inputRate = 100;

            bundle.classMap.c31.flow.inputRate = 3500;
            bundle.classMap.c32.flow.inputRate =  500;
            bundle.classMap.c33.flow.inputRate = 1000;
        },
        complete: function (bundle) {
            assert(bundle.classMap.match_any.flow.bandwidth.toFixed(0) == 0);

            // under bandwidth
            assert(bundle.classMap.c31.flow.bandwidth.toFixed(0) == 5000);

            // under bandwidth
            assert(bundle.classMap.c32.flow.bandwidth.toFixed(0) == 1000);
            assert(bundle.classMap.c33.flow.bandwidth.toFixed(0) == 1000);
        }
    });

    run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.match_any.flow.inputRate = 100;

            bundle.classMap.c31.flow.inputRate = 3400;
            bundle.classMap.c32.flow.inputRate =  500;
            bundle.classMap.c33.flow.inputRate = 1000;
        },
        complete: function (bundle) {
            assert(bundle.classMap.match_any.flow.bandwidth.toFixed(0) == 5000);

            // under bandwidth
            assert(bundle.classMap.c31.flow.bandwidth.toFixed(0) == 5000);

            // under bandwidth
            assert(bundle.classMap.c32.flow.bandwidth.toFixed(0) == 1000);
            assert(bundle.classMap.c33.flow.bandwidth.toFixed(0) == 1000);
        }
    });

    run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.c31.flow.inputRate = 5000;
            bundle.classMap.c32.flow.inputRate = 5000;
            bundle.classMap.c33.flow.inputRate = 5000;
        },
        complete: function (bundle) {
            assert(bundle.classMap.match_any.flow.bandwidth.toFixed(0) == 5000);

            assert(bundle.classMap.c31.flow.bandwidth.toFixed(0) == 3000);
            assert(bundle.classMap.c32.flow.bandwidth.toFixed(0) == 1000);
            assert(bundle.classMap.c33.flow.bandwidth.toFixed(0) == 1000);
        }
    });

}) ();
