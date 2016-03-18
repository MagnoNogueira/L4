
MYAPP = {};

MYAPP.utility = {};

MYAPP.utility = (function () {
    var assert = function assert(exp, message) {
        if (!exp) {
            throw new AssertException(message);
        }
    };

    function AssertException(message) {
        this.message = message;
    }

    AssertException.prototype.toString = function () {
        return 'AssertException: ' + this.message;
    }

    return {
        assert: assert
    };
}());

MYAPP.bundleRateLimit = (function () {
    var defaultSetup = function (bundle) {
        var i, name, classMap, newRate, bundlePerPriority = {};

        for (name in bundle.classMap) {
            if (!bundle.classMap.hasOwnProperty(name)) {
                continue;
            }

            classMap = bundle.classMap[ name ];

            classMap.flow.bandwidth = classMap.bandwidth;
            classMap.flow.inputRate = 0;
        }
    },
    createSortBundle = function (bundle) {
        var name, classMap,
            sortBundle = [],
            i;

        for (name in bundle.classMap) {
            if (!bundle.classMap.hasOwnProperty(name)) {
                continue;
            }

            classMap = bundle.classMap[ name ];

            if (!sortBundle[classMap.priority]) {
                sortBundle[classMap.priority] = {
                    priority: classMap.priority,
                    classMap: {},
                    length: 0,
                    totalInputRate: 0
                };
            }

            sortBundle[classMap.priority].classMap[name] = classMap;
            sortBundle[classMap.priority].length += 1;

            if (classMap.bandwidth < classMap.flow.inputRate) {
                classMap.flow.inputRate = classMap.bandwidth;
            }

            sortBundle[classMap.priority].totalInputRate += classMap.flow.inputRate;
        }

        sortBundle.sort(function(a, b){
            return b.priority - a.priority;
        });

        for (i = 0; i < sortBundle.length; i += 1) {
            if (typeof sortBundle[i] === 'undefined') {
                sortBundle.length = i;
                break;
            }
        }

        return sortBundle;
    },
    calcNewBandwidth = function (bundle) {
        var i, name, classMap, newRate,
            bundleRate = bundle.bandwidth,
            priority,
            bundlePerPriority;

        for (i = 0; i < bundle.length; i += 1) {
            bundlePerPriority = bundle[i];

            for (name in bundlePerPriority.classMap) {

                if (!bundlePerPriority.classMap.hasOwnProperty(name)) {
                    continue;
                }

                classMap = bundlePerPriority.classMap[name];

                if (bundlePerPriority.totalInputRate <= bundleRate ||
                    bundlePerPriority.totalInputRate === 0 ||
                    classMap.flow.input_rate === 0) {
                    newRate = classMap.bandwidth;
                }
                else {
                    newRate = classMap.flow.inputRate * bundleRate / bundlePerPriority.totalInputRate;
                }

                if (classMap.bandwidth < newRate) {
                    newRate = classMap.bandwidth;
                }

                classMap.flow.bandwidth = newRate;
            }

            bundleRate -= bundlePerPriority.totalInputRate;
            if (bundleRate < 0) {
                bundleRate = 0;
            }
        }
    },
    run = function (obj) {
        var bundle = obj.bundle,
            sortBundle = [];

        defaultSetup(bundle);
        if (obj.setup) {
            obj.setup(bundle);
        }

        //
        // main function
        //
        sortBundle = createSortBundle(bundle);
        sortBundle.bandwidth = bundle.bandwidth;

        calcNewBandwidth(sortBundle);

        //

        if (obj.complete) {
            obj.complete(bundle);
        }
    };

    return {
        run: run
    };

}());

(function() {

    var bundleRateLimit = MYAPP.bundleRateLimit,
        assert = MYAPP.utility.assert,
        bundle = {};

    bundle.bandwidth = 5000;
    bundle.classMap = {};

    bundle.classMap.match_any = {
        name: "match-any",
        bandwidth: 5000,
        priority: 0,
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
        priority: 10,
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
        priority: 40,
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
        priority: 40,
        flow: {
            inputRate: 0,
            bandwidth: 0
        },
        match: {
            vlan: 4001,
            cos: 4
        }
    };

    bundleRateLimit.run({
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

    bundleRateLimit.run({
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

    bundleRateLimit.run({
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

    bundleRateLimit.run({
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

    bundleRateLimit.run({
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

    bundleRateLimit.run({
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

    bundleRateLimit.run({
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

    bundleRateLimit.run({
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

    bundleRateLimit.run({
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

    bundleRateLimit.run({
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

    bundleRateLimit.run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.match_any.flow.inputRate = 0;
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

    bundleRateLimit.run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.match_any.flow.inputRate = 5000;
            bundle.classMap.c31.flow.inputRate = 5000;
            bundle.classMap.c32.flow.inputRate = 5000;
            bundle.classMap.c33.flow.inputRate = 5000;
        },
        complete: function (bundle) {
            assert(bundle.classMap.match_any.flow.bandwidth.toFixed(0) == 0);
            assert(bundle.classMap.c31.flow.bandwidth.toFixed(0) == 3000);
            assert(bundle.classMap.c32.flow.bandwidth.toFixed(0) == 1000);
            assert(bundle.classMap.c33.flow.bandwidth.toFixed(0) == 1000);
        }
    });

}());

(function() {

    var bundleRateLimit = MYAPP.bundleRateLimit,
        assert = MYAPP.utility.assert,
        bundle = {};

    bundle.bandwidth = 5000;
    bundle.classMap = {};

    bundle.classMap.match_any = {
        name: "match-any",
        bandwidth: 6000,
        priority: 0,
        flow: {
            inputRate: 0,
            bandwidth: 0
        },
        match: {
        }
    };

    bundleRateLimit.run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.match_any.flow.inputRate = 7000;
        },
        complete: function (bundle) {
            assert(bundle.classMap.match_any.flow.bandwidth.toFixed(0) == 5000);
        }
    });

}());

(function() {

    var bundleRateLimit = MYAPP.bundleRateLimit,
        assert = MYAPP.utility.assert,
        bundle = {};

    bundle.bandwidth = 5000;
    bundle.classMap = {};

    bundle.classMap.c32 = {
        name: "c32",
        bandwidth: 6000,
        priority: 40,
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
        bandwidth: 6000,
        priority: 40,
        flow: {
            inputRate: 0,
            bandwidth: 0
        },
        match: {
            vlan: 4001,
            cos: 4
        }
    };

    bundleRateLimit.run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.c32.flow.inputRate = 7000;
            bundle.classMap.c33.flow.inputRate = 0;
        },
        complete: function (bundle) {
            assert(bundle.classMap.c32.flow.bandwidth.toFixed(0) == 5000);
            assert(bundle.classMap.c33.flow.bandwidth.toFixed(0) == 0);
        }
    });

    bundleRateLimit.run({
        bundle: bundle,
        setup: function(bundle) {
            bundle.classMap.c32.flow.inputRate = 7000;
            bundle.classMap.c33.flow.inputRate = 7000;
        },
        complete: function (bundle) {
            assert(bundle.classMap.c32.flow.bandwidth.toFixed(0) == 2500);
            assert(bundle.classMap.c33.flow.bandwidth.toFixed(0) == 2500);
        }
    });

}());
