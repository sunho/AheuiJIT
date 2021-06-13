webpackHotUpdate_N_E(1,{

/***/ "./components/CodeRunner.js":
/*!**********************************!*\
  !*** ./components/CodeRunner.js ***!
  \**********************************/
/*! exports provided: default */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
eval("__webpack_require__.r(__webpack_exports__);\n/* WEBPACK VAR INJECTION */(function(module) {/* harmony import */ var react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! react/jsx-dev-runtime */ \"./node_modules/react/jsx-dev-runtime.js\");\n/* harmony import */ var react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__);\n/* harmony import */ var _Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_slicedToArray__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! ./node_modules/@babel/runtime/helpers/esm/slicedToArray */ \"./node_modules/@babel/runtime/helpers/esm/slicedToArray.js\");\n/* harmony import */ var _Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_classCallCheck__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! ./node_modules/@babel/runtime/helpers/esm/classCallCheck */ \"./node_modules/@babel/runtime/helpers/esm/classCallCheck.js\");\n/* harmony import */ var _Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_createClass__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! ./node_modules/@babel/runtime/helpers/esm/createClass */ \"./node_modules/@babel/runtime/helpers/esm/createClass.js\");\n/* harmony import */ var _Examples__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(/*! ./Examples */ \"./components/Examples.js\");\n/* harmony import */ var _Config__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(/*! ./Config */ \"./components/Config.js\");\n/* harmony import */ var aheui_jit__WEBPACK_IMPORTED_MODULE_6__ = __webpack_require__(/*! aheui-jit */ \"./node_modules/aheui-jit/dist/index.js\");\n/* harmony import */ var aheui_jit__WEBPACK_IMPORTED_MODULE_6___default = /*#__PURE__*/__webpack_require__.n(aheui_jit__WEBPACK_IMPORTED_MODULE_6__);\n/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_7__ = __webpack_require__(/*! react */ \"./node_modules/react/index.js\");\n/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_7___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_7__);\n\n\n\n\n\n\nvar _jsxFileName = \"/Users/sunho/Documents/dev/AheuiJIT/webdemo/components/CodeRunner.js\",\n    _s = $RefreshSig$(),\n    _s2 = $RefreshSig$();\n\n\n\n\n\nvar PAGE_SIZE = 2048;\n\nvar StdoutBuffer = /*#__PURE__*/function () {\n  function StdoutBuffer() {\n    Object(_Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_classCallCheck__WEBPACK_IMPORTED_MODULE_2__[\"default\"])(this, StdoutBuffer);\n\n    this.buffer = \"\";\n    this.page = 1;\n\n    this.onFlush = function (x) {};\n  }\n\n  Object(_Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_createClass__WEBPACK_IMPORTED_MODULE_3__[\"default\"])(StdoutBuffer, [{\n    key: \"addMsg\",\n    value: function addMsg(msg) {\n      this.buffer += msg;\n      this.update();\n    }\n  }, {\n    key: \"flush\",\n    value: function flush() {\n      this.onFlush(this.buffer);\n      this.buffer = \"\";\n      this.page = 1;\n    }\n  }, {\n    key: \"update\",\n    value: function update() {\n      if (this.buffer.length > this.page * PAGE_SIZE) {\n        //this.onFlush(this.buffer);\n        this.page++;\n      }\n    }\n  }]);\n\n  return StdoutBuffer;\n}();\n\n;\n\nvar StdinBuffer = /*#__PURE__*/function () {\n  function StdinBuffer() {\n    Object(_Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_classCallCheck__WEBPACK_IMPORTED_MODULE_2__[\"default\"])(this, StdinBuffer);\n\n    this.buffer = \"\";\n    this.cursor = 0;\n  }\n\n  Object(_Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_createClass__WEBPACK_IMPORTED_MODULE_3__[\"default\"])(StdinBuffer, [{\n    key: \"setBuffer\",\n    value: function setBuffer(buf) {\n      this.buffer = buf;\n      this.cursor = 0;\n    }\n  }, {\n    key: \"readNum\",\n    value: function readNum() {\n      var isBlank = function isBlank(x) {\n        return x === '\\n' || x === ' ';\n      };\n\n      var meetNum = false;\n      var start;\n\n      for (; this.cursor < this.buffer.length; ++this.cursor) {\n        var ch = this.buffer.charAt(this.cursor);\n\n        if ('0' <= ch && ch <= '9') {\n          if (!meetNum) {\n            start = this.cursor;\n          }\n\n          meetNum = true;\n        } else if (isBlank(ch)) {\n          if (meetNum) {\n            break;\n          }\n\n          continue;\n        } else {\n          break;\n        }\n      }\n\n      if (!meetNum) {\n        throw \"No number\";\n      }\n\n      return Number.parseInt(this.buffer.slice(start, this.cursor));\n    }\n  }, {\n    key: \"readChar\",\n    value: function readChar() {\n      if (this.cursor >= this.buffer.length) {\n        return 'eof';\n      }\n\n      var ch = this.buffer.charAt(this.cursor);\n\n      if (ch === '\\\\' && this.cursor + 1 < this.buffer.length && this.buffer.charAt(this.cursor + 1) === '0') {\n        this.cursor += 2;\n        return 'eof';\n      }\n\n      ++this.cursor;\n      return ch;\n    }\n  }]);\n\n  return StdinBuffer;\n}();\n\n;\nvar stdio = new StdoutBuffer();\nvar stdin = new StdinBuffer();\n\naheui_jit__WEBPACK_IMPORTED_MODULE_6___default.a.callbacks.printChar = function (msg) {\n  stdio.addMsg(msg);\n};\n\naheui_jit__WEBPACK_IMPORTED_MODULE_6___default.a.callbacks.printNum = function (num) {\n  stdio.addMsg(num.toString());\n};\n\naheui_jit__WEBPACK_IMPORTED_MODULE_6___default.a.callbacks.inputNum = function () {\n  return stdin.readNum();\n};\n\naheui_jit__WEBPACK_IMPORTED_MODULE_6___default.a.callbacks.inputChar = function () {\n  return stdin.readChar();\n};\n\nfunction useInput(_ref) {\n  _s();\n\n  var className = _ref.className,\n      defaultValue = _ref.defaultValue;\n\n  var _useState = Object(react__WEBPACK_IMPORTED_MODULE_7__[\"useState\"])(defaultValue),\n      value = _useState[0],\n      setValue = _useState[1];\n\n  var input = /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"textarea\", {\n    className: className,\n    value: value,\n    onChange: function onChange(e) {\n      return setValue(e.target.value);\n    }\n  }, void 0, false, {\n    fileName: _jsxFileName,\n    lineNumber: 108,\n    columnNumber: 17\n  }, this);\n\n  return [value, input, setValue];\n}\n\n_s(useInput, \"+di++irDcPspjmhJVP9frUKGzpo=\");\n\nvar runn = fasle;\n\nfunction CodeRunner() {\n  _s2();\n\n  var _useState2 = Object(react__WEBPACK_IMPORTED_MODULE_7__[\"useState\"])(false),\n      running = _useState2[0],\n      setRunning = _useState2[1];\n\n  var _useState3 = Object(react__WEBPACK_IMPORTED_MODULE_7__[\"useState\"])(false),\n      isReady = _useState3[0],\n      setIsReady = _useState3[1];\n\n  var _useState4 = Object(react__WEBPACK_IMPORTED_MODULE_7__[\"useState\"])(''),\n      out = _useState4[0],\n      setOut = _useState4[1];\n\n  var _useInput = useInput({\n    className: \"Code\",\n    defaultValue: \"밤밣따빠밣밟따뿌\\n빠맣파빨받밤뚜뭏\\n돋밬탕빠맣붏두붇\\n볻뫃박발뚷투뭏붖\\n뫃도뫃희멓뭏뭏붘\\n뫃봌토범더벌뿌뚜\\n뽑뽀멓멓더벓뻐뚠\\n뽀덩벐멓뻐덕더벅\"\n  }),\n      _useInput2 = Object(_Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_slicedToArray__WEBPACK_IMPORTED_MODULE_1__[\"default\"])(_useInput, 3),\n      code = _useInput2[0],\n      codeInput = _useInput2[1],\n      setCodeInput = _useInput2[2];\n\n  var _useInput3 = useInput({\n    className: \"Input\",\n    defaultValue: \"\"\n  }),\n      _useInput4 = Object(_Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_slicedToArray__WEBPACK_IMPORTED_MODULE_1__[\"default\"])(_useInput3, 3),\n      input = _useInput4[0],\n      inputInput = _useInput4[1],\n      _setInput = _useInput4[2];\n\n  Object(react__WEBPACK_IMPORTED_MODULE_7__[\"useEffect\"])(function () {\n    aheui_jit__WEBPACK_IMPORTED_MODULE_6___default.a.ready.then(function () {\n      setIsReady(true);\n    });\n  }, []);\n  Object(react__WEBPACK_IMPORTED_MODULE_7__[\"useEffect\"])(function () {\n    stdio.onFlush = function (x) {\n      setOut(x);\n    };\n  }, []);\n  return /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"Fragment\"], {\n    children: [/*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n      className: \"Ready\",\n      children: [\"(ready: \", isReady ? \"yes\" : \"no\", \")\"]\n    }, void 0, true, {\n      fileName: _jsxFileName,\n      lineNumber: 132,\n      columnNumber: 7\n    }, this), /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n      children: /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(_Examples__WEBPACK_IMPORTED_MODULE_4__[\"default\"], {\n        setCode: function setCode(code) {\n          setCodeInput(code);\n        },\n        setInput: function setInput(i) {\n          _setInput(i);\n        }\n      }, void 0, false, {\n        fileName: _jsxFileName,\n        lineNumber: 134,\n        columnNumber: 9\n      }, this)\n    }, void 0, false, {\n      fileName: _jsxFileName,\n      lineNumber: 133,\n      columnNumber: 7\n    }, this), /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n      className: \"Container\",\n      children: [/*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n        children: [/*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n          children: \"input\"\n        }, void 0, false, {\n          fileName: _jsxFileName,\n          lineNumber: 138,\n          columnNumber: 11\n        }, this), inputInput]\n      }, void 0, true, {\n        fileName: _jsxFileName,\n        lineNumber: 137,\n        columnNumber: 9\n      }, this), /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n        children: [/*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n          children: \"code\"\n        }, void 0, false, {\n          fileName: _jsxFileName,\n          lineNumber: 142,\n          columnNumber: 11\n        }, this), codeInput]\n      }, void 0, true, {\n        fileName: _jsxFileName,\n        lineNumber: 141,\n        columnNumber: 9\n      }, this)]\n    }, void 0, true, {\n      fileName: _jsxFileName,\n      lineNumber: 136,\n      columnNumber: 7\n    }, this), /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n      children: !running && /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n        className: \"RunButton\",\n        onClick: function onClick() {\n          if (!isReady || code.length === 0) return;\n          setRunning(true);\n          stdin.setBuffer(input);\n          aheui_jit__WEBPACK_IMPORTED_MODULE_6___default.a.run(code).then(function () {\n            stdio.flush();\n            setRunning(false);\n          });\n        },\n        children: \"RUN\"\n      }, void 0, false, {\n        fileName: _jsxFileName,\n        lineNumber: 147,\n        columnNumber: 21\n      }, this)\n    }, void 0, false, {\n      fileName: _jsxFileName,\n      lineNumber: 146,\n      columnNumber: 7\n    }, this), /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n      className: \"Out\",\n      children: out\n    }, void 0, false, {\n      fileName: _jsxFileName,\n      lineNumber: 159,\n      columnNumber: 7\n    }, this), /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(_Config__WEBPACK_IMPORTED_MODULE_5__[\"default\"], {}, void 0, false, {\n      fileName: _jsxFileName,\n      lineNumber: 161,\n      columnNumber: 8\n    }, this)]\n  }, void 0, true);\n}\n\n_s2(CodeRunner, \"S5gV72xsG8N035cbwlrBRlEGPXc=\", false, function () {\n  return [useInput, useInput];\n});\n\n_c = CodeRunner;\n/* harmony default export */ __webpack_exports__[\"default\"] = (CodeRunner);\n\nvar _c;\n\n$RefreshReg$(_c, \"CodeRunner\");\n\n;\n    var _a, _b;\n    // Legacy CSS implementations will `eval` browser code in a Node.js context\n    // to extract CSS. For backwards compatibility, we need to check we're in a\n    // browser context before continuing.\n    if (typeof self !== 'undefined' &&\n        // AMP / No-JS mode does not inject these helpers:\n        '$RefreshHelpers$' in self) {\n        var currentExports = module.__proto__.exports;\n        var prevExports = (_b = (_a = module.hot.data) === null || _a === void 0 ? void 0 : _a.prevExports) !== null && _b !== void 0 ? _b : null;\n        // This cannot happen in MainTemplate because the exports mismatch between\n        // templating and execution.\n        self.$RefreshHelpers$.registerExportsForReactRefresh(currentExports, module.i);\n        // A module can be accepted automatically based on its exports, e.g. when\n        // it is a Refresh Boundary.\n        if (self.$RefreshHelpers$.isReactRefreshBoundary(currentExports)) {\n            // Save the previous exports on update so we can compare the boundary\n            // signatures.\n            module.hot.dispose(function (data) {\n                data.prevExports = currentExports;\n            });\n            // Unconditionally accept an update to this module, we'll check if it's\n            // still a Refresh Boundary later.\n            module.hot.accept();\n            // This field is set when the previous version of this module was a\n            // Refresh Boundary, letting us know we need to check for invalidation or\n            // enqueue an update.\n            if (prevExports !== null) {\n                // A boundary can become ineligible if its exports are incompatible\n                // with the previous exports.\n                //\n                // For example, if you add/remove/change exports, we'll want to\n                // re-execute the importing modules, and force those components to\n                // re-render. Similarly, if you convert a class component to a\n                // function, we want to invalidate the boundary.\n                if (self.$RefreshHelpers$.shouldInvalidateReactRefreshBoundary(prevExports, currentExports)) {\n                    module.hot.invalidate();\n                }\n                else {\n                    self.$RefreshHelpers$.scheduleUpdate();\n                }\n            }\n        }\n        else {\n            // Since we just executed the code for the module, it's possible that the\n            // new exports made it ineligible for being a boundary.\n            // We only care about the case when we were _previously_ a boundary,\n            // because we already accepted this update (accidental side effect).\n            var isNoLongerABoundary = prevExports !== null;\n            if (isNoLongerABoundary) {\n                module.hot.invalidate();\n            }\n        }\n    }\n\n/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../node_modules/next/dist/compiled/webpack/harmony-module.js */ \"./node_modules/next/dist/compiled/webpack/harmony-module.js\")(module)))//# sourceURL=[module]\n//# sourceMappingURL=data:application/json;charset=utf-8;base64,eyJ2ZXJzaW9uIjozLCJzb3VyY2VzIjpbIndlYnBhY2s6Ly9fTl9FLy4vY29tcG9uZW50cy9Db2RlUnVubmVyLmpzP2ZlMDIiXSwibmFtZXMiOlsiUEFHRV9TSVpFIiwiU3Rkb3V0QnVmZmVyIiwiYnVmZmVyIiwicGFnZSIsIm9uRmx1c2giLCJ4IiwibXNnIiwidXBkYXRlIiwibGVuZ3RoIiwiU3RkaW5CdWZmZXIiLCJjdXJzb3IiLCJidWYiLCJpc0JsYW5rIiwibWVldE51bSIsInN0YXJ0IiwiY2giLCJjaGFyQXQiLCJOdW1iZXIiLCJwYXJzZUludCIsInNsaWNlIiwic3RkaW8iLCJzdGRpbiIsIkFoZXVpSklUIiwiY2FsbGJhY2tzIiwicHJpbnRDaGFyIiwiYWRkTXNnIiwicHJpbnROdW0iLCJudW0iLCJ0b1N0cmluZyIsImlucHV0TnVtIiwicmVhZE51bSIsImlucHV0Q2hhciIsInJlYWRDaGFyIiwidXNlSW5wdXQiLCJjbGFzc05hbWUiLCJkZWZhdWx0VmFsdWUiLCJ1c2VTdGF0ZSIsInZhbHVlIiwic2V0VmFsdWUiLCJpbnB1dCIsImUiLCJ0YXJnZXQiLCJydW5uIiwiZmFzbGUiLCJDb2RlUnVubmVyIiwicnVubmluZyIsInNldFJ1bm5pbmciLCJpc1JlYWR5Iiwic2V0SXNSZWFkeSIsIm91dCIsInNldE91dCIsImNvZGUiLCJjb2RlSW5wdXQiLCJzZXRDb2RlSW5wdXQiLCJpbnB1dElucHV0Iiwic2V0SW5wdXQiLCJ1c2VFZmZlY3QiLCJyZWFkeSIsInRoZW4iLCJpIiwic2V0QnVmZmVyIiwicnVuIiwiZmx1c2giXSwibWFwcGluZ3MiOiI7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7QUFBQTtBQUNBO0FBQ0E7QUFDQTtBQUVBLElBQU1BLFNBQVMsR0FBRyxJQUFsQjs7SUFFTUMsWTtBQUNKLDBCQUFjO0FBQUE7O0FBQ1osU0FBS0MsTUFBTCxHQUFjLEVBQWQ7QUFDQSxTQUFLQyxJQUFMLEdBQVksQ0FBWjs7QUFDQSxTQUFLQyxPQUFMLEdBQWUsVUFBQ0MsQ0FBRCxFQUFPLENBQUUsQ0FBeEI7QUFDRDs7OzsyQkFFTUMsRyxFQUFLO0FBQ1YsV0FBS0osTUFBTCxJQUFlSSxHQUFmO0FBQ0EsV0FBS0MsTUFBTDtBQUNEOzs7NEJBRU07QUFDTCxXQUFLSCxPQUFMLENBQWEsS0FBS0YsTUFBbEI7QUFDQSxXQUFLQSxNQUFMLEdBQWMsRUFBZDtBQUNBLFdBQUtDLElBQUwsR0FBWSxDQUFaO0FBQ0Q7Ozs2QkFFUTtBQUNQLFVBQUksS0FBS0QsTUFBTCxDQUFZTSxNQUFaLEdBQXFCLEtBQUtMLElBQUwsR0FBWUgsU0FBckMsRUFBZ0Q7QUFDOUM7QUFDQSxhQUFLRyxJQUFMO0FBQ0Q7QUFDRjs7Ozs7O0FBQ0Y7O0lBRUtNLFc7QUFDSix5QkFBYztBQUFBOztBQUNaLFNBQUtQLE1BQUwsR0FBYyxFQUFkO0FBQ0EsU0FBS1EsTUFBTCxHQUFjLENBQWQ7QUFDRDs7Ozs4QkFFU0MsRyxFQUFLO0FBQ2IsV0FBS1QsTUFBTCxHQUFjUyxHQUFkO0FBQ0EsV0FBS0QsTUFBTCxHQUFjLENBQWQ7QUFDRDs7OzhCQUVTO0FBQ1IsVUFBTUUsT0FBTyxHQUFHLFNBQVZBLE9BQVUsQ0FBQ1AsQ0FBRCxFQUFPO0FBQ3JCLGVBQU9BLENBQUMsS0FBSyxJQUFOLElBQWNBLENBQUMsS0FBSyxHQUEzQjtBQUNELE9BRkQ7O0FBR0EsVUFBSVEsT0FBTyxHQUFHLEtBQWQ7QUFDQSxVQUFJQyxLQUFKOztBQUNBLGFBQU0sS0FBS0osTUFBTCxHQUFjLEtBQUtSLE1BQUwsQ0FBWU0sTUFBaEMsRUFBd0MsRUFBRSxLQUFLRSxNQUEvQyxFQUF1RDtBQUNyRCxZQUFNSyxFQUFFLEdBQUcsS0FBS2IsTUFBTCxDQUFZYyxNQUFaLENBQW1CLEtBQUtOLE1BQXhCLENBQVg7O0FBQ0EsWUFBSSxPQUFPSyxFQUFQLElBQWFBLEVBQUUsSUFBSSxHQUF2QixFQUE0QjtBQUMxQixjQUFJLENBQUNGLE9BQUwsRUFBYztBQUNaQyxpQkFBSyxHQUFHLEtBQUtKLE1BQWI7QUFDRDs7QUFDREcsaUJBQU8sR0FBRyxJQUFWO0FBQ0QsU0FMRCxNQUtPLElBQUlELE9BQU8sQ0FBQ0csRUFBRCxDQUFYLEVBQWlCO0FBQ3RCLGNBQUlGLE9BQUosRUFBYTtBQUNYO0FBQ0Q7O0FBQ0Q7QUFDRCxTQUxNLE1BS0E7QUFDTDtBQUNEO0FBQ0Y7O0FBQ0QsVUFBSSxDQUFDQSxPQUFMLEVBQWM7QUFDWixjQUFNLFdBQU47QUFDRDs7QUFDRCxhQUFPSSxNQUFNLENBQUNDLFFBQVAsQ0FBZ0IsS0FBS2hCLE1BQUwsQ0FBWWlCLEtBQVosQ0FBa0JMLEtBQWxCLEVBQXlCLEtBQUtKLE1BQTlCLENBQWhCLENBQVA7QUFDRDs7OytCQUVVO0FBQ1QsVUFBSSxLQUFLQSxNQUFMLElBQWUsS0FBS1IsTUFBTCxDQUFZTSxNQUEvQixFQUF1QztBQUNyQyxlQUFPLEtBQVA7QUFDRDs7QUFDRCxVQUFNTyxFQUFFLEdBQUcsS0FBS2IsTUFBTCxDQUFZYyxNQUFaLENBQW1CLEtBQUtOLE1BQXhCLENBQVg7O0FBQ0EsVUFBSUssRUFBRSxLQUFLLElBQVAsSUFBZSxLQUFLTCxNQUFMLEdBQWMsQ0FBZCxHQUFrQixLQUFLUixNQUFMLENBQVlNLE1BQTdDLElBQXVELEtBQUtOLE1BQUwsQ0FBWWMsTUFBWixDQUFtQixLQUFLTixNQUFMLEdBQVksQ0FBL0IsTUFBc0MsR0FBakcsRUFBc0c7QUFDcEcsYUFBS0EsTUFBTCxJQUFlLENBQWY7QUFDQSxlQUFPLEtBQVA7QUFDRDs7QUFDRCxRQUFFLEtBQUtBLE1BQVA7QUFDQSxhQUFPSyxFQUFQO0FBQ0Q7Ozs7OztBQUNGO0FBRUQsSUFBTUssS0FBSyxHQUFHLElBQUluQixZQUFKLEVBQWQ7QUFDQSxJQUFNb0IsS0FBSyxHQUFHLElBQUlaLFdBQUosRUFBZDs7QUFFQWEsZ0RBQVEsQ0FBQ0MsU0FBVCxDQUFtQkMsU0FBbkIsR0FBK0IsVUFBQWxCLEdBQUcsRUFBSTtBQUNwQ2MsT0FBSyxDQUFDSyxNQUFOLENBQWFuQixHQUFiO0FBQ0QsQ0FGRDs7QUFJQWdCLGdEQUFRLENBQUNDLFNBQVQsQ0FBbUJHLFFBQW5CLEdBQThCLFVBQUFDLEdBQUcsRUFBSTtBQUNuQ1AsT0FBSyxDQUFDSyxNQUFOLENBQWFFLEdBQUcsQ0FBQ0MsUUFBSixFQUFiO0FBQ0QsQ0FGRDs7QUFJQU4sZ0RBQVEsQ0FBQ0MsU0FBVCxDQUFtQk0sUUFBbkIsR0FBOEIsWUFBTTtBQUNsQyxTQUFPUixLQUFLLENBQUNTLE9BQU4sRUFBUDtBQUNELENBRkQ7O0FBSUFSLGdEQUFRLENBQUNDLFNBQVQsQ0FBbUJRLFNBQW5CLEdBQStCLFlBQU07QUFDbkMsU0FBT1YsS0FBSyxDQUFDVyxRQUFOLEVBQVA7QUFDRCxDQUZEOztBQUlBLFNBQVNDLFFBQVQsT0FBK0M7QUFBQTs7QUFBQSxNQUEzQkMsU0FBMkIsUUFBM0JBLFNBQTJCO0FBQUEsTUFBaEJDLFlBQWdCLFFBQWhCQSxZQUFnQjs7QUFBQSxrQkFDbkJDLHNEQUFRLENBQUNELFlBQUQsQ0FEVztBQUFBLE1BQ3RDRSxLQURzQztBQUFBLE1BQy9CQyxRQUQrQjs7QUFFN0MsTUFBTUMsS0FBSyxnQkFBRztBQUFVLGFBQVMsRUFBRUwsU0FBckI7QUFBZ0MsU0FBSyxFQUFFRyxLQUF2QztBQUE4QyxZQUFRLEVBQUUsa0JBQUFHLENBQUM7QUFBQSxhQUFJRixRQUFRLENBQUNFLENBQUMsQ0FBQ0MsTUFBRixDQUFTSixLQUFWLENBQVo7QUFBQTtBQUF6RDtBQUFBO0FBQUE7QUFBQTtBQUFBLFVBQWQ7O0FBQ0EsU0FBTyxDQUFDQSxLQUFELEVBQVFFLEtBQVIsRUFBZUQsUUFBZixDQUFQO0FBQ0Q7O0dBSlFMLFE7O0FBTVQsSUFBSVMsSUFBSSxHQUFHQyxLQUFYOztBQUVBLFNBQVNDLFVBQVQsR0FBc0I7QUFBQTs7QUFBQSxtQkFDVVIsc0RBQVEsQ0FBQyxLQUFELENBRGxCO0FBQUEsTUFDYlMsT0FEYTtBQUFBLE1BQ0pDLFVBREk7O0FBQUEsbUJBRVVWLHNEQUFRLENBQUMsS0FBRCxDQUZsQjtBQUFBLE1BRWJXLE9BRmE7QUFBQSxNQUVKQyxVQUZJOztBQUFBLG1CQUdFWixzREFBUSxDQUFDLEVBQUQsQ0FIVjtBQUFBLE1BR2JhLEdBSGE7QUFBQSxNQUdSQyxNQUhROztBQUFBLGtCQUlvQmpCLFFBQVEsQ0FBQztBQUFFQyxhQUFTLEVBQUUsTUFBYjtBQUFxQkMsZ0JBQVksRUFBRTtBQUFuQyxHQUFELENBSjVCO0FBQUE7QUFBQSxNQUliZ0IsSUFKYTtBQUFBLE1BSVBDLFNBSk87QUFBQSxNQUlJQyxZQUpKOztBQUFBLG1CQUtrQnBCLFFBQVEsQ0FBQztBQUFFQyxhQUFTLEVBQUUsT0FBYjtBQUFzQkMsZ0JBQVksRUFBRTtBQUFwQyxHQUFELENBTDFCO0FBQUE7QUFBQSxNQUtiSSxLQUxhO0FBQUEsTUFLTmUsVUFMTTtBQUFBLE1BS01DLFNBTE47O0FBTXBCQyx5REFBUyxDQUFDLFlBQU07QUFDZGxDLG9EQUFRLENBQUNtQyxLQUFULENBQWVDLElBQWYsQ0FBb0IsWUFBTTtBQUN4QlYsZ0JBQVUsQ0FBQyxJQUFELENBQVY7QUFDRCxLQUZEO0FBR0QsR0FKUSxFQUlQLEVBSk8sQ0FBVDtBQU1BUSx5REFBUyxDQUFDLFlBQU07QUFDZHBDLFNBQUssQ0FBQ2hCLE9BQU4sR0FBZ0IsVUFBQUMsQ0FBQyxFQUFJO0FBQUU2QyxZQUFNLENBQUM3QyxDQUFELENBQU47QUFBWSxLQUFuQztBQUNELEdBRlEsRUFFUCxFQUZPLENBQVQ7QUFJQSxzQkFDRTtBQUFBLDRCQUNFO0FBQUssZUFBUyxFQUFDLE9BQWY7QUFBQSw2QkFBZ0MwQyxPQUFPLEdBQUcsS0FBSCxHQUFXLElBQWxEO0FBQUE7QUFBQTtBQUFBO0FBQUE7QUFBQSxZQURGLGVBRUU7QUFBQSw2QkFDRSxxRUFBQyxpREFBRDtBQUFVLGVBQU8sRUFBRSxpQkFBQ0ksSUFBRCxFQUFVO0FBQUVFLHNCQUFZLENBQUNGLElBQUQsQ0FBWjtBQUFvQixTQUFuRDtBQUFxRCxnQkFBUSxFQUFFLGtCQUFDUSxDQUFELEVBQU87QUFBRUosbUJBQVEsQ0FBQ0ksQ0FBRCxDQUFSO0FBQWM7QUFBdEY7QUFBQTtBQUFBO0FBQUE7QUFBQTtBQURGO0FBQUE7QUFBQTtBQUFBO0FBQUEsWUFGRixlQUtFO0FBQUssZUFBUyxFQUFDLFdBQWY7QUFBQSw4QkFDRTtBQUFBLGdDQUNFO0FBQUE7QUFBQTtBQUFBO0FBQUE7QUFBQTtBQUFBLGdCQURGLEVBRUdMLFVBRkg7QUFBQTtBQUFBO0FBQUE7QUFBQTtBQUFBLGNBREYsZUFLRTtBQUFBLGdDQUNFO0FBQUE7QUFBQTtBQUFBO0FBQUE7QUFBQTtBQUFBLGdCQURGLEVBRUdGLFNBRkg7QUFBQTtBQUFBO0FBQUE7QUFBQTtBQUFBLGNBTEY7QUFBQTtBQUFBO0FBQUE7QUFBQTtBQUFBLFlBTEYsZUFlRTtBQUFBLGdCQUNDLENBQUNQLE9BQUQsaUJBQWE7QUFBSyxpQkFBUyxFQUFDLFdBQWY7QUFBMkIsZUFBTyxFQUFFLG1CQUFNO0FBQ3RELGNBQUksQ0FBQ0UsT0FBRCxJQUFZSSxJQUFJLENBQUMzQyxNQUFMLEtBQWdCLENBQWhDLEVBQ0U7QUFDRnNDLG9CQUFVLENBQUMsSUFBRCxDQUFWO0FBQ0F6QixlQUFLLENBQUN1QyxTQUFOLENBQWdCckIsS0FBaEI7QUFDQWpCLDBEQUFRLENBQUN1QyxHQUFULENBQWFWLElBQWIsRUFBbUJPLElBQW5CLENBQXdCLFlBQU07QUFDNUJ0QyxpQkFBSyxDQUFDMEMsS0FBTjtBQUNBaEIsc0JBQVUsQ0FBQyxLQUFELENBQVY7QUFDRCxXQUhEO0FBSUEsU0FUWTtBQUFBO0FBQUE7QUFBQTtBQUFBO0FBQUE7QUFBQTtBQURkO0FBQUE7QUFBQTtBQUFBO0FBQUEsWUFmRixlQTRCRTtBQUFLLGVBQVMsRUFBQyxLQUFmO0FBQUEsZ0JBQXNCRztBQUF0QjtBQUFBO0FBQUE7QUFBQTtBQUFBLFlBNUJGLGVBOEJHLHFFQUFDLCtDQUFEO0FBQUE7QUFBQTtBQUFBO0FBQUEsWUE5Qkg7QUFBQSxrQkFERjtBQWtDRDs7SUFsRFFMLFU7VUFJaUNYLFEsRUFDRkEsUTs7O0tBTC9CVyxVO0FBb0RNQSx5RUFBZiIsImZpbGUiOiIuL2NvbXBvbmVudHMvQ29kZVJ1bm5lci5qcy5qcyIsInNvdXJjZXNDb250ZW50IjpbImltcG9ydCBFeGFtcGxlcyBmcm9tICcuL0V4YW1wbGVzJztcbmltcG9ydCBDb25maWcgZnJvbSAnLi9Db25maWcnO1xuaW1wb3J0IEFoZXVpSklUIGZyb20gJ2FoZXVpLWppdCc7XG5pbXBvcnQgeyB1c2VFZmZlY3QsIHVzZVN0YXRlIH0gZnJvbSAncmVhY3QnO1xuXG5jb25zdCBQQUdFX1NJWkUgPSAyMDQ4O1xuXG5jbGFzcyBTdGRvdXRCdWZmZXIge1xuICBjb25zdHJ1Y3RvcigpIHtcbiAgICB0aGlzLmJ1ZmZlciA9IFwiXCI7XG4gICAgdGhpcy5wYWdlID0gMTtcbiAgICB0aGlzLm9uRmx1c2ggPSAoeCkgPT4ge307XG4gIH1cblxuICBhZGRNc2cobXNnKSB7XG4gICAgdGhpcy5idWZmZXIgKz0gbXNnO1xuICAgIHRoaXMudXBkYXRlKCk7XG4gIH1cblxuICBmbHVzaCgpe1xuICAgIHRoaXMub25GbHVzaCh0aGlzLmJ1ZmZlcik7XG4gICAgdGhpcy5idWZmZXIgPSBcIlwiO1xuICAgIHRoaXMucGFnZSA9IDE7XG4gIH1cblxuICB1cGRhdGUoKSB7XG4gICAgaWYgKHRoaXMuYnVmZmVyLmxlbmd0aCA+IHRoaXMucGFnZSAqIFBBR0VfU0laRSkge1xuICAgICAgLy90aGlzLm9uRmx1c2godGhpcy5idWZmZXIpO1xuICAgICAgdGhpcy5wYWdlICsrO1xuICAgIH1cbiAgfVxufTtcblxuY2xhc3MgU3RkaW5CdWZmZXIge1xuICBjb25zdHJ1Y3RvcigpIHtcbiAgICB0aGlzLmJ1ZmZlciA9IFwiXCI7XG4gICAgdGhpcy5jdXJzb3IgPSAwO1xuICB9XG5cbiAgc2V0QnVmZmVyKGJ1Zikge1xuICAgIHRoaXMuYnVmZmVyID0gYnVmO1xuICAgIHRoaXMuY3Vyc29yID0gMDtcbiAgfVxuXG4gIHJlYWROdW0oKSB7XG4gICAgY29uc3QgaXNCbGFuayA9ICh4KSA9PiB7XG4gICAgICByZXR1cm4geCA9PT0gJ1xcbicgfHwgeCA9PT0gJyAnO1xuICAgIH1cbiAgICBsZXQgbWVldE51bSA9IGZhbHNlO1xuICAgIGxldCBzdGFydDtcbiAgICBmb3IgKDt0aGlzLmN1cnNvciA8IHRoaXMuYnVmZmVyLmxlbmd0aDsgKyt0aGlzLmN1cnNvcikge1xuICAgICAgY29uc3QgY2ggPSB0aGlzLmJ1ZmZlci5jaGFyQXQodGhpcy5jdXJzb3IpO1xuICAgICAgaWYgKCcwJyA8PSBjaCAmJiBjaCA8PSAnOScpIHtcbiAgICAgICAgaWYgKCFtZWV0TnVtKSB7XG4gICAgICAgICAgc3RhcnQgPSB0aGlzLmN1cnNvcjtcbiAgICAgICAgfVxuICAgICAgICBtZWV0TnVtID0gdHJ1ZTtcbiAgICAgIH0gZWxzZSBpZiAoaXNCbGFuayhjaCkpIHtcbiAgICAgICAgaWYgKG1lZXROdW0pIHtcbiAgICAgICAgICBicmVhaztcbiAgICAgICAgfVxuICAgICAgICBjb250aW51ZTtcbiAgICAgIH0gZWxzZSB7XG4gICAgICAgIGJyZWFrO1xuICAgICAgfVxuICAgIH1cbiAgICBpZiAoIW1lZXROdW0pIHtcbiAgICAgIHRocm93IFwiTm8gbnVtYmVyXCI7XG4gICAgfVxuICAgIHJldHVybiBOdW1iZXIucGFyc2VJbnQodGhpcy5idWZmZXIuc2xpY2Uoc3RhcnQsIHRoaXMuY3Vyc29yKSk7XG4gIH1cblxuICByZWFkQ2hhcigpIHtcbiAgICBpZiAodGhpcy5jdXJzb3IgPj0gdGhpcy5idWZmZXIubGVuZ3RoKSB7XG4gICAgICByZXR1cm4gJ2VvZic7XG4gICAgfVxuICAgIGNvbnN0IGNoID0gdGhpcy5idWZmZXIuY2hhckF0KHRoaXMuY3Vyc29yKTtcbiAgICBpZiAoY2ggPT09ICdcXFxcJyAmJiB0aGlzLmN1cnNvciArIDEgPCB0aGlzLmJ1ZmZlci5sZW5ndGggJiYgdGhpcy5idWZmZXIuY2hhckF0KHRoaXMuY3Vyc29yKzEpID09PSAnMCcpIHtcbiAgICAgIHRoaXMuY3Vyc29yICs9IDI7XG4gICAgICByZXR1cm4gJ2VvZic7XG4gICAgfVxuICAgICsrdGhpcy5jdXJzb3I7XG4gICAgcmV0dXJuIGNoO1xuICB9XG59O1xuXG5jb25zdCBzdGRpbyA9IG5ldyBTdGRvdXRCdWZmZXIoKTtcbmNvbnN0IHN0ZGluID0gbmV3IFN0ZGluQnVmZmVyKCk7XG5cbkFoZXVpSklULmNhbGxiYWNrcy5wcmludENoYXIgPSBtc2cgPT4geyBcbiAgc3RkaW8uYWRkTXNnKG1zZyk7XG59O1xuXG5BaGV1aUpJVC5jYWxsYmFja3MucHJpbnROdW0gPSBudW0gPT4geyBcbiAgc3RkaW8uYWRkTXNnKG51bS50b1N0cmluZygpKTtcbn07XG5cbkFoZXVpSklULmNhbGxiYWNrcy5pbnB1dE51bSA9ICgpID0+IHsgXG4gIHJldHVybiBzdGRpbi5yZWFkTnVtKCk7XG59O1xuXG5BaGV1aUpJVC5jYWxsYmFja3MuaW5wdXRDaGFyID0gKCkgPT4geyBcbiAgcmV0dXJuIHN0ZGluLnJlYWRDaGFyKCk7XG59O1xuXG5mdW5jdGlvbiB1c2VJbnB1dCh7IGNsYXNzTmFtZSwgZGVmYXVsdFZhbHVlIH0pIHtcbiAgY29uc3QgW3ZhbHVlLCBzZXRWYWx1ZV0gPSB1c2VTdGF0ZShkZWZhdWx0VmFsdWUpO1xuICBjb25zdCBpbnB1dCA9IDx0ZXh0YXJlYSBjbGFzc05hbWU9e2NsYXNzTmFtZX0gdmFsdWU9e3ZhbHVlfSBvbkNoYW5nZT17ZSA9PiBzZXRWYWx1ZShlLnRhcmdldC52YWx1ZSl9ICAvPjtcbiAgcmV0dXJuIFt2YWx1ZSwgaW5wdXQsIHNldFZhbHVlXTtcbn1cblxudmFyIHJ1bm4gPSBmYXNsZTtcblxuZnVuY3Rpb24gQ29kZVJ1bm5lcigpIHtcbiAgY29uc3QgW3J1bm5pbmcsIHNldFJ1bm5pbmddID0gdXNlU3RhdGUoZmFsc2UpO1xuICBjb25zdCBbaXNSZWFkeSwgc2V0SXNSZWFkeV0gPSB1c2VTdGF0ZShmYWxzZSk7XG4gIGNvbnN0IFtvdXQsIHNldE91dF0gPSB1c2VTdGF0ZSgnJyk7XG4gIGNvbnN0IFtjb2RlLCBjb2RlSW5wdXQsIHNldENvZGVJbnB1dF0gPSB1c2VJbnB1dCh7IGNsYXNzTmFtZTogXCJDb2RlXCIsIGRlZmF1bHRWYWx1ZTogXCLrsKTrsKPrlLDruaDrsKPrsJ/rlLDrv4xcXG7ruaDrp6PtjIzruajrsJvrsKTrmpzrrY9cXG7rj4vrsKztg5XruaDrp6Prto/rkZDrtodcXG7rs7vrq4PrsJXrsJzrmrftiKzrrY/rtpZcXG7rq4Prj4Trq4PtnazrqZPrrY/rrY/rtphcXG7rq4PrtIzthqDrspTrjZTrsozrv4zrmpxcXG7rvZHrvYDrqZPrqZPrjZTrspPru5DrmqBcXG7rvYDrjanrspDrqZPru5DrjZXrjZTrsoVcIiB9KTtcbiAgY29uc3QgW2lucHV0LCBpbnB1dElucHV0LCBzZXRJbnB1dF0gPSB1c2VJbnB1dCh7IGNsYXNzTmFtZTogXCJJbnB1dFwiLCBkZWZhdWx0VmFsdWU6IFwiXCIgfSk7XG4gIHVzZUVmZmVjdCgoKSA9PiB7XG4gICAgQWhldWlKSVQucmVhZHkudGhlbigoKSA9PiB7XG4gICAgICBzZXRJc1JlYWR5KHRydWUpO1xuICAgIH0pO1xuICB9LFtdKTtcblxuICB1c2VFZmZlY3QoKCkgPT4ge1xuICAgIHN0ZGlvLm9uRmx1c2ggPSB4ID0+IHsgc2V0T3V0KHgpOyB9O1xuICB9LFtdKTtcblxuICByZXR1cm4gKFxuICAgIDw+XG4gICAgICA8ZGl2IGNsYXNzTmFtZT1cIlJlYWR5XCI+KHJlYWR5OiB7aXNSZWFkeSA/IFwieWVzXCIgOiBcIm5vXCJ9KTwvZGl2PlxuICAgICAgPGRpdj5cbiAgICAgICAgPEV4YW1wbGVzIHNldENvZGU9eyhjb2RlKSA9PiB7IHNldENvZGVJbnB1dChjb2RlKTt9fSBzZXRJbnB1dD17KGkpID0+IHsgc2V0SW5wdXQoaSk7IH19Lz5cbiAgICAgIDwvZGl2PlxuICAgICAgPGRpdiBjbGFzc05hbWU9XCJDb250YWluZXJcIj5cbiAgICAgICAgPGRpdj5cbiAgICAgICAgICA8ZGl2PmlucHV0PC9kaXY+XG4gICAgICAgICAge2lucHV0SW5wdXR9XG4gICAgICAgIDwvZGl2PlxuICAgICAgICA8ZGl2PlxuICAgICAgICAgIDxkaXY+Y29kZTwvZGl2PlxuICAgICAgICAgIHtjb2RlSW5wdXR9XG4gICAgICAgIDwvZGl2PlxuICAgICAgPC9kaXY+XG4gICAgICA8ZGl2PlxuICAgICAgeyFydW5uaW5nICYmICg8ZGl2IGNsYXNzTmFtZT1cIlJ1bkJ1dHRvblwiIG9uQ2xpY2s9eygpID0+IHsgXG4gICAgICAgIGlmICghaXNSZWFkeSB8fCBjb2RlLmxlbmd0aCA9PT0gMClcbiAgICAgICAgICByZXR1cm47XG4gICAgICAgIHNldFJ1bm5pbmcodHJ1ZSk7XG4gICAgICAgIHN0ZGluLnNldEJ1ZmZlcihpbnB1dCk7XG4gICAgICAgIEFoZXVpSklULnJ1bihjb2RlKS50aGVuKCgpID0+IHtcbiAgICAgICAgICBzdGRpby5mbHVzaCgpO1xuICAgICAgICAgIHNldFJ1bm5pbmcoZmFsc2UpO1xuICAgICAgICB9KTtcbiAgICAgICB9fT5SVU48L2Rpdj5cbiAgICAgICl9XG4gICAgICAgPC9kaXY+XG4gICAgICA8ZGl2IGNsYXNzTmFtZT1cIk91dFwiPntvdXR9PC9kaXY+XG5cbiAgICAgICA8Q29uZmlnLz5cbiAgICA8Lz5cbiAgKTtcbn1cblxuZXhwb3J0IGRlZmF1bHQgQ29kZVJ1bm5lcjsiXSwic291cmNlUm9vdCI6IiJ9\n//# sourceURL=webpack-internal:///./components/CodeRunner.js\n");

/***/ })

})