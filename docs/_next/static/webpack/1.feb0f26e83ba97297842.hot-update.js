webpackHotUpdate_N_E(1,{

/***/ "./components/CodeRunner.js":
/*!**********************************!*\
  !*** ./components/CodeRunner.js ***!
  \**********************************/
/*! exports provided: default */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
eval("__webpack_require__.r(__webpack_exports__);\n/* WEBPACK VAR INJECTION */(function(module) {/* harmony import */ var react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! react/jsx-dev-runtime */ \"./node_modules/react/jsx-dev-runtime.js\");\n/* harmony import */ var react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0___default = /*#__PURE__*/__webpack_require__.n(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__);\n/* harmony import */ var _Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_slicedToArray__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! ./node_modules/@babel/runtime/helpers/esm/slicedToArray */ \"./node_modules/@babel/runtime/helpers/esm/slicedToArray.js\");\n/* harmony import */ var _Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_classCallCheck__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! ./node_modules/@babel/runtime/helpers/esm/classCallCheck */ \"./node_modules/@babel/runtime/helpers/esm/classCallCheck.js\");\n/* harmony import */ var _Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_createClass__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! ./node_modules/@babel/runtime/helpers/esm/createClass */ \"./node_modules/@babel/runtime/helpers/esm/createClass.js\");\n/* harmony import */ var _Examples__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(/*! ./Examples */ \"./components/Examples.js\");\n/* harmony import */ var _Config__WEBPACK_IMPORTED_MODULE_5__ = __webpack_require__(/*! ./Config */ \"./components/Config.js\");\n/* harmony import */ var aheui_jit__WEBPACK_IMPORTED_MODULE_6__ = __webpack_require__(/*! aheui-jit */ \"./node_modules/aheui-jit/dist/index.js\");\n/* harmony import */ var aheui_jit__WEBPACK_IMPORTED_MODULE_6___default = /*#__PURE__*/__webpack_require__.n(aheui_jit__WEBPACK_IMPORTED_MODULE_6__);\n/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_7__ = __webpack_require__(/*! react */ \"./node_modules/react/index.js\");\n/* harmony import */ var react__WEBPACK_IMPORTED_MODULE_7___default = /*#__PURE__*/__webpack_require__.n(react__WEBPACK_IMPORTED_MODULE_7__);\n\n\n\n\n\n\nvar _jsxFileName = \"/Users/sunho/Documents/dev/AheuiJIT/webdemo/components/CodeRunner.js\",\n    _s = $RefreshSig$(),\n    _s2 = $RefreshSig$();\n\n\n\n\n\nvar PAGE_SIZE = 2048;\n\nvar StdoutBuffer = /*#__PURE__*/function () {\n  function StdoutBuffer() {\n    Object(_Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_classCallCheck__WEBPACK_IMPORTED_MODULE_2__[\"default\"])(this, StdoutBuffer);\n\n    this.buffer = \"\";\n    this.page = 1;\n\n    this.onFlush = function (x) {};\n  }\n\n  Object(_Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_createClass__WEBPACK_IMPORTED_MODULE_3__[\"default\"])(StdoutBuffer, [{\n    key: \"addMsg\",\n    value: function addMsg(msg) {\n      this.buffer += msg;\n      this.update();\n    }\n  }, {\n    key: \"flush\",\n    value: function flush() {\n      this.onFlush(this.buffer);\n      this.buffer = \"\";\n      this.page = 1;\n    }\n  }, {\n    key: \"update\",\n    value: function update() {\n      if (this.buffer.length > this.page * PAGE_SIZE) {\n        //this.onFlush(this.buffer);\n        this.page++;\n      }\n    }\n  }]);\n\n  return StdoutBuffer;\n}();\n\n;\n\nvar StdinBuffer = /*#__PURE__*/function () {\n  function StdinBuffer() {\n    Object(_Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_classCallCheck__WEBPACK_IMPORTED_MODULE_2__[\"default\"])(this, StdinBuffer);\n\n    this.buffer = \"\";\n    this.cursor = 0;\n  }\n\n  Object(_Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_createClass__WEBPACK_IMPORTED_MODULE_3__[\"default\"])(StdinBuffer, [{\n    key: \"setBuffer\",\n    value: function setBuffer(buf) {\n      this.buffer = buf;\n      this.cursor = 0;\n    }\n  }, {\n    key: \"readNum\",\n    value: function readNum() {\n      var isBlank = function isBlank(x) {\n        return x === '\\n' || x === ' ';\n      };\n\n      var meetNum = false;\n      var start;\n\n      for (; this.cursor < this.buffer.length; ++this.cursor) {\n        var ch = this.buffer.charAt(this.cursor);\n\n        if ('0' <= ch && ch <= '9') {\n          if (!meetNum) {\n            start = this.cursor;\n          }\n\n          meetNum = true;\n        } else if (isBlank(ch)) {\n          if (meetNum) {\n            break;\n          }\n\n          continue;\n        } else {\n          break;\n        }\n      }\n\n      if (!meetNum) {\n        throw \"No number\";\n      }\n\n      return Number.parseInt(this.buffer.slice(start, this.cursor));\n    }\n  }, {\n    key: \"readChar\",\n    value: function readChar() {\n      if (this.cursor >= this.buffer.length) {\n        return 'eof';\n      }\n\n      var ch = this.buffer.charAt(this.cursor);\n\n      if (ch === '\\\\' && this.cursor + 1 < this.buffer.length && this.buffer.charAt(this.cursor + 1) === '0') {\n        this.cursor += 2;\n        return 'eof';\n      }\n\n      ++this.cursor;\n      return ch;\n    }\n  }]);\n\n  return StdinBuffer;\n}();\n\n;\nvar stdio = new StdoutBuffer();\nvar stdin = new StdinBuffer();\n\naheui_jit__WEBPACK_IMPORTED_MODULE_6___default.a.callbacks.printChar = function (msg) {\n  stdio.addMsg(msg);\n};\n\naheui_jit__WEBPACK_IMPORTED_MODULE_6___default.a.callbacks.printNum = function (num) {\n  stdio.addMsg(num.toString());\n};\n\naheui_jit__WEBPACK_IMPORTED_MODULE_6___default.a.callbacks.inputNum = function () {\n  return stdin.readNum();\n};\n\naheui_jit__WEBPACK_IMPORTED_MODULE_6___default.a.callbacks.inputChar = function () {\n  return stdin.readChar();\n};\n\nfunction useInput(_ref) {\n  _s();\n\n  var className = _ref.className,\n      defaultValue = _ref.defaultValue;\n\n  var _useState = Object(react__WEBPACK_IMPORTED_MODULE_7__[\"useState\"])(defaultValue),\n      value = _useState[0],\n      setValue = _useState[1];\n\n  var input = /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"textarea\", {\n    className: className,\n    value: value,\n    onChange: function onChange(e) {\n      return setValue(e.target.value);\n    }\n  }, void 0, false, {\n    fileName: _jsxFileName,\n    lineNumber: 108,\n    columnNumber: 17\n  }, this);\n\n  return [value, input, setValue];\n}\n\n_s(useInput, \"+di++irDcPspjmhJVP9frUKGzpo=\");\n\nfunction CodeRunner() {\n  _s2();\n\n  var _useState2 = Object(react__WEBPACK_IMPORTED_MODULE_7__[\"useState\"])(false),\n      running = _useState2[0],\n      setRunning = _useState2[1];\n\n  var _useState3 = Object(react__WEBPACK_IMPORTED_MODULE_7__[\"useState\"])(false),\n      isReady = _useState3[0],\n      setIsReady = _useState3[1];\n\n  var _useState4 = Object(react__WEBPACK_IMPORTED_MODULE_7__[\"useState\"])(''),\n      out = _useState4[0],\n      setOut = _useState4[1];\n\n  var _useInput = useInput({\n    className: \"Code\",\n    defaultValue: \"밤밣따빠밣밟따뿌\\n빠맣파빨받밤뚜뭏\\n돋밬탕빠맣붏두붇\\n볻뫃박발뚷투뭏붖\\n뫃도뫃희멓뭏뭏붘\\n뫃봌토범더벌뿌뚜\\n뽑뽀멓멓더벓뻐뚠\\n뽀덩벐멓뻐덕더벅\"\n  }),\n      _useInput2 = Object(_Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_slicedToArray__WEBPACK_IMPORTED_MODULE_1__[\"default\"])(_useInput, 3),\n      code = _useInput2[0],\n      codeInput = _useInput2[1],\n      setCodeInput = _useInput2[2];\n\n  var _useInput3 = useInput({\n    className: \"Input\",\n    defaultValue: \"\"\n  }),\n      _useInput4 = Object(_Users_sunho_Documents_dev_AheuiJIT_webdemo_node_modules_babel_runtime_helpers_esm_slicedToArray__WEBPACK_IMPORTED_MODULE_1__[\"default\"])(_useInput3, 3),\n      input = _useInput4[0],\n      inputInput = _useInput4[1],\n      _setInput = _useInput4[2];\n\n  Object(react__WEBPACK_IMPORTED_MODULE_7__[\"useEffect\"])(function () {\n    aheui_jit__WEBPACK_IMPORTED_MODULE_6___default.a.ready.then(function () {\n      setIsReady(true);\n    });\n  }, []);\n  Object(react__WEBPACK_IMPORTED_MODULE_7__[\"useEffect\"])(function () {\n    stdio.onFlush = function (x) {\n      setOut(x);\n    };\n  }, []);\n  return /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"Fragment\"], {\n    children: [/*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n      className: \"Ready\",\n      children: [\"(ready: \", isReady ? \"yes\" : \"no\", \")\"]\n    }, void 0, true, {\n      fileName: _jsxFileName,\n      lineNumber: 130,\n      columnNumber: 7\n    }, this), /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n      children: /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(_Examples__WEBPACK_IMPORTED_MODULE_4__[\"default\"], {\n        setCode: function setCode(code) {\n          setCodeInput(code);\n        },\n        setInput: function setInput(i) {\n          _setInput(i);\n        }\n      }, void 0, false, {\n        fileName: _jsxFileName,\n        lineNumber: 132,\n        columnNumber: 9\n      }, this)\n    }, void 0, false, {\n      fileName: _jsxFileName,\n      lineNumber: 131,\n      columnNumber: 7\n    }, this), /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n      className: \"Container\",\n      children: [/*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n        children: [/*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n          children: \"input\"\n        }, void 0, false, {\n          fileName: _jsxFileName,\n          lineNumber: 136,\n          columnNumber: 11\n        }, this), inputInput]\n      }, void 0, true, {\n        fileName: _jsxFileName,\n        lineNumber: 135,\n        columnNumber: 9\n      }, this), /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n        children: [/*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n          children: \"code\"\n        }, void 0, false, {\n          fileName: _jsxFileName,\n          lineNumber: 140,\n          columnNumber: 11\n        }, this), codeInput]\n      }, void 0, true, {\n        fileName: _jsxFileName,\n        lineNumber: 139,\n        columnNumber: 9\n      }, this)]\n    }, void 0, true, {\n      fileName: _jsxFileName,\n      lineNumber: 134,\n      columnNumber: 7\n    }, this), /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n      children: !running && /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n        className: \"RunButton\",\n        onClick: function onClick() {\n          if (!isReady || code.length === 0) return;\n          setRunning(true);\n          stdin.setBuffer(input);\n          aheui_jit__WEBPACK_IMPORTED_MODULE_6___default.a.run(code).then(function () {\n            stdio.flush();\n          });\n        },\n        children: \"RUN\"\n      }, void 0, false, {\n        fileName: _jsxFileName,\n        lineNumber: 145,\n        columnNumber: 21\n      }, this)\n    }, void 0, false, {\n      fileName: _jsxFileName,\n      lineNumber: 144,\n      columnNumber: 7\n    }, this), /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(\"div\", {\n      className: \"Out\",\n      children: out\n    }, void 0, false, {\n      fileName: _jsxFileName,\n      lineNumber: 156,\n      columnNumber: 7\n    }, this), /*#__PURE__*/Object(react_jsx_dev_runtime__WEBPACK_IMPORTED_MODULE_0__[\"jsxDEV\"])(_Config__WEBPACK_IMPORTED_MODULE_5__[\"default\"], {}, void 0, false, {\n      fileName: _jsxFileName,\n      lineNumber: 158,\n      columnNumber: 8\n    }, this)]\n  }, void 0, true);\n}\n\n_s2(CodeRunner, \"S5gV72xsG8N035cbwlrBRlEGPXc=\", false, function () {\n  return [useInput, useInput];\n});\n\n_c = CodeRunner;\n/* harmony default export */ __webpack_exports__[\"default\"] = (CodeRunner);\n\nvar _c;\n\n$RefreshReg$(_c, \"CodeRunner\");\n\n;\n    var _a, _b;\n    // Legacy CSS implementations will `eval` browser code in a Node.js context\n    // to extract CSS. For backwards compatibility, we need to check we're in a\n    // browser context before continuing.\n    if (typeof self !== 'undefined' &&\n        // AMP / No-JS mode does not inject these helpers:\n        '$RefreshHelpers$' in self) {\n        var currentExports = module.__proto__.exports;\n        var prevExports = (_b = (_a = module.hot.data) === null || _a === void 0 ? void 0 : _a.prevExports) !== null && _b !== void 0 ? _b : null;\n        // This cannot happen in MainTemplate because the exports mismatch between\n        // templating and execution.\n        self.$RefreshHelpers$.registerExportsForReactRefresh(currentExports, module.i);\n        // A module can be accepted automatically based on its exports, e.g. when\n        // it is a Refresh Boundary.\n        if (self.$RefreshHelpers$.isReactRefreshBoundary(currentExports)) {\n            // Save the previous exports on update so we can compare the boundary\n            // signatures.\n            module.hot.dispose(function (data) {\n                data.prevExports = currentExports;\n            });\n            // Unconditionally accept an update to this module, we'll check if it's\n            // still a Refresh Boundary later.\n            module.hot.accept();\n            // This field is set when the previous version of this module was a\n            // Refresh Boundary, letting us know we need to check for invalidation or\n            // enqueue an update.\n            if (prevExports !== null) {\n                // A boundary can become ineligible if its exports are incompatible\n                // with the previous exports.\n                //\n                // For example, if you add/remove/change exports, we'll want to\n                // re-execute the importing modules, and force those components to\n                // re-render. Similarly, if you convert a class component to a\n                // function, we want to invalidate the boundary.\n                if (self.$RefreshHelpers$.shouldInvalidateReactRefreshBoundary(prevExports, currentExports)) {\n                    module.hot.invalidate();\n                }\n                else {\n                    self.$RefreshHelpers$.scheduleUpdate();\n                }\n            }\n        }\n        else {\n            // Since we just executed the code for the module, it's possible that the\n            // new exports made it ineligible for being a boundary.\n            // We only care about the case when we were _previously_ a boundary,\n            // because we already accepted this update (accidental side effect).\n            var isNoLongerABoundary = prevExports !== null;\n            if (isNoLongerABoundary) {\n                module.hot.invalidate();\n            }\n        }\n    }\n\n/* WEBPACK VAR INJECTION */}.call(this, __webpack_require__(/*! ./../node_modules/next/dist/compiled/webpack/harmony-module.js */ \"./node_modules/next/dist/compiled/webpack/harmony-module.js\")(module)))//# sourceURL=[module]\n//# sourceMappingURL=data:application/json;charset=utf-8;base64,eyJ2ZXJzaW9uIjozLCJzb3VyY2VzIjpbIndlYnBhY2s6Ly9fTl9FLy4vY29tcG9uZW50cy9Db2RlUnVubmVyLmpzP2ZlMDIiXSwibmFtZXMiOlsiUEFHRV9TSVpFIiwiU3Rkb3V0QnVmZmVyIiwiYnVmZmVyIiwicGFnZSIsIm9uRmx1c2giLCJ4IiwibXNnIiwidXBkYXRlIiwibGVuZ3RoIiwiU3RkaW5CdWZmZXIiLCJjdXJzb3IiLCJidWYiLCJpc0JsYW5rIiwibWVldE51bSIsInN0YXJ0IiwiY2giLCJjaGFyQXQiLCJOdW1iZXIiLCJwYXJzZUludCIsInNsaWNlIiwic3RkaW8iLCJzdGRpbiIsIkFoZXVpSklUIiwiY2FsbGJhY2tzIiwicHJpbnRDaGFyIiwiYWRkTXNnIiwicHJpbnROdW0iLCJudW0iLCJ0b1N0cmluZyIsImlucHV0TnVtIiwicmVhZE51bSIsImlucHV0Q2hhciIsInJlYWRDaGFyIiwidXNlSW5wdXQiLCJjbGFzc05hbWUiLCJkZWZhdWx0VmFsdWUiLCJ1c2VTdGF0ZSIsInZhbHVlIiwic2V0VmFsdWUiLCJpbnB1dCIsImUiLCJ0YXJnZXQiLCJDb2RlUnVubmVyIiwicnVubmluZyIsInNldFJ1bm5pbmciLCJpc1JlYWR5Iiwic2V0SXNSZWFkeSIsIm91dCIsInNldE91dCIsImNvZGUiLCJjb2RlSW5wdXQiLCJzZXRDb2RlSW5wdXQiLCJpbnB1dElucHV0Iiwic2V0SW5wdXQiLCJ1c2VFZmZlY3QiLCJyZWFkeSIsInRoZW4iLCJpIiwic2V0QnVmZmVyIiwicnVuIiwiZmx1c2giXSwibWFwcGluZ3MiOiI7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7QUFBQTtBQUNBO0FBQ0E7QUFDQTtBQUVBLElBQU1BLFNBQVMsR0FBRyxJQUFsQjs7SUFFTUMsWTtBQUNKLDBCQUFjO0FBQUE7O0FBQ1osU0FBS0MsTUFBTCxHQUFjLEVBQWQ7QUFDQSxTQUFLQyxJQUFMLEdBQVksQ0FBWjs7QUFDQSxTQUFLQyxPQUFMLEdBQWUsVUFBQ0MsQ0FBRCxFQUFPLENBQUUsQ0FBeEI7QUFDRDs7OzsyQkFFTUMsRyxF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QTtBQUFBLGdCQURGLEVBRUdMLFVBRkg7QUFBQTtBQUFBO0FBQUE7QUFBQTtBQUFBLGNBREYsZUFLRTtBQUFBLGdDQUNFO0FBQUE7QUFBQTtBQUFBO0FBQUE7QUFBQTtBQUFBLGdCQURGLEVBRUdGLFNBRkg7QUFBQTtBQUFBO0FBQUE7QUFBQTtBQUFBLGNBTEY7QUFBQTtBQUFBO0FBQUE7QUFBQTtBQUFBLFlBTEYsZUFlRTtBQUFBLGdCQUNDLENBQUNQLE9BQUQsaUJBQWE7QUFBSyxpQkFBUyxFQUFDLFdBQWY7QUFBMkIsZUFBTyxFQUFFLG1CQUFNO0FBQ3RELGNBQUksQ0FBQ0UsT0FBRCxJQUFZSSxJQUFJLENBQUN6QyxNQUFMLEtBQWdCLENBQWhDLEVBQ0U7QUFDRm9DLG9CQUFVLENBQUMsSUFBRCxDQUFWO0FBQ0F2QixlQUFLLENBQUNxQyxTQUFOLENBQWdCbkIsS0FBaEI7QUFDQWpCLDBEQUFRLENBQUNxQyxHQUFULENBQWFWLElBQWIsRUFBbUJPLElBQW5CLENBQXdCLFlBQU07QUFDNUJwQyxpQkFBSyxDQUFDd0MsS0FBTjtBQUNELFdBRkQ7QUFHQSxTQVJZO0FBQUE7QUFBQTtBQUFBO0FBQUE7QUFBQTtBQUFBO0FBRGQ7QUFBQTtBQUFBO0FBQUE7QUFBQSxZQWZGLGVBMkJFO0FBQUssZUFBUyxFQUFDLEtBQWY7QUFBQSxnQkFBc0JiO0FBQXRCO0FBQUE7QUFBQTtBQUFBO0FBQUEsWUEzQkYsZUE2QkcscUVBQUMsK0NBQUQ7QUFBQTtBQUFBO0FBQUE7QUFBQSxZQTdCSDtBQUFBLGtCQURGO0FBaUNEOztJQWpEUUwsVTtVQUlpQ1QsUSxFQUNGQSxROzs7S0FML0JTLFU7QUFtRE1BLHlFQUFmIiwiZmlsZSI6Ii4vY29tcG9uZW50cy9Db2RlUnVubmVyLmpzLmpzIiwic291cmNlc0NvbnRlbnQiOlsiaW1wb3J0IEV4YW1wbGVzIGZyb20gJy4vRXhhbXBsZXMnO1xuaW1wb3J0IENvbmZpZyBmcm9tICcuL0NvbmZpZyc7XG5pbXBvcnQgQWhldWlKSVQgZnJvbSAnYWhldWktaml0JztcbmltcG9ydCB7IHVzZUVmZmVjdCwgdXNlU3RhdGUgfSBmcm9tICdyZWFjdCc7XG5cbmNvbnN0IFBBR0VfU0laRSA9IDIwNDg7XG5cbmNsYXNzIFN0ZG91dEJ1ZmZlciB7XG4gIGNvbnN0cnVjdG9yKCkge1xuICAgIHRoaXMuYnVmZmVyID0gXCJcIjtcbiAgICB0aGlzLnBhZ2UgPSAxO1xuICAgIHRoaXMub25GbHVzaCA9ICh4KSA9PiB7fTtcbiAgfVxuXG4gIGFkZE1zZyhtc2cpIHtcbiAgICB0aGlzLmJ1ZmZlciArPSBtc2c7XG4gICAgdGhpcy51cGRhdGUoKTtcbiAgfVxuXG4gIGZsdXNoKCl7XG4gICAgdGhpcy5vbkZsdXNoKHRoaXMuYnVmZmVyKTtcbiAgICB0aGlzLmJ1ZmZlciA9IFwiXCI7XG4gICAgdGhpcy5wYWdlID0gMTtcbiAgfVxuXG4gIHVwZGF0ZSgpIHtcbiAgICBpZiAodGhpcy5idWZmZXIubGVuZ3RoID4gdGhpcy5wYWdlICogUEFHRV9TSVpFKSB7XG4gICAgICAvL3RoaXMub25GbHVzaCh0aGlzLmJ1ZmZlcik7XG4gICAgICB0aGlzLnBhZ2UgKys7XG4gICAgfVxuICB9XG59O1xuXG5jbGFzcyBTdGRpbkJ1ZmZlciB7XG4gIGNvbnN0cnVjdG9yKCkge1xuICAgIHRoaXMuYnVmZmVyID0gXCJcIjtcbiAgICB0aGlzLmN1cnNvciA9IDA7XG4gIH1cblxuICBzZXRCdWZmZXIoYnVmKSB7XG4gICAgdGhpcy5idWZmZXIgPSBidWY7XG4gICAgdGhpcy5jdXJzb3IgPSAwO1xuICB9XG5cbiAgcmVhZE51bSgpIHtcbiAgICBjb25zdCBpc0JsYW5rID0gKHgpID0+IHtcbiAgICAgIHJldHVybiB4ID09PSAnXFxuJyB8fCB4ID09PSAnICc7XG4gICAgfVxuICAgIGxldCBtZWV0TnVtID0gZmFsc2U7XG4gICAgbGV0IHN0YXJ0O1xuICAgIGZvciAoO3RoaXMuY3Vyc29yIDwgdGhpcy5idWZmZXIubGVuZ3RoOyArK3RoaXMuY3Vyc29yKSB7XG4gICAgICBjb25zdCBjaCA9IHRoaXMuYnVmZmVyLmNoYXJBdCh0aGlzLmN1cnNvcik7XG4gICAgICBpZiAoJzAnIDw9IGNoICYmIGNoIDw9ICc5Jykge1xuICAgICAgICBpZiAoIW1lZXROdW0pIHtcbiAgICAgICAgICBzdGFydCA9IHRoaXMuY3Vyc29yO1xuICAgICAgICB9XG4gICAgICAgIG1lZXROdW0gPSB0cnVlO1xuICAgICAgfSBlbHNlIGlmIChpc0JsYW5rKGNoKSkge1xuICAgICAgICBpZiAobWVldE51bSkge1xuICAgICAgICAgIGJyZWFrO1xuICAgICAgICB9XG4gICAgICAgIGNvbnRpbnVlO1xuICAgICAgfSBlbHNlIHtcbiAgICAgICAgYnJlYWs7XG4gICAgICB9XG4gICAgfVxuICAgIGlmICghbWVldE51bSkge1xuICAgICAgdGhyb3cgXCJObyBudW1iZXJcIjtcbiAgICB9XG4gICAgcmV0dXJuIE51bWJlci5wYXJzZUludCh0aGlzLmJ1ZmZlci5zbGljZShzdGFydCwgdGhpcy5jdXJzb3IpKTtcbiAgfVxuXG4gIHJlYWRDaGFyKCkge1xuICAgIGlmICh0aGlzLmN1cnNvciA+PSB0aGlzLmJ1ZmZlci5sZW5ndGgpIHtcbiAgICAgIHJldHVybiAnZW9mJztcbiAgICB9XG4gICAgY29uc3QgY2ggPSB0aGlzLmJ1ZmZlci5jaGFyQXQodGhpcy5jdXJzb3IpO1xuICAgIGlmIChjaCA9PT0gJ1xcXFwnICYmIHRoaXMuY3Vyc29yICsgMSA8IHRoaXMuYnVmZmVyLmxlbmd0aCAmJiB0aGlzLmJ1ZmZlci5jaGFyQXQodGhpcy5jdXJzb3IrMSkgPT09ICcwJykge1xuICAgICAgdGhpcy5jdXJzb3IgKz0gMjtcbiAgICAgIHJldHVybiAnZW9mJztcbiAgICB9XG4gICAgKyt0aGlzLmN1cnNvcjtcbiAgICByZXR1cm4gY2g7XG4gIH1cbn07XG5cbmNvbnN0IHN0ZGlvID0gbmV3IFN0ZG91dEJ1ZmZlcigpO1xuY29uc3Qgc3RkaW4gPSBuZXcgU3RkaW5CdWZmZXIoKTtcblxuQWhldWlKSVQuY2FsbGJhY2tzLnByaW50Q2hhciA9IG1zZyA9PiB7IFxuICBzdGRpby5hZGRNc2cobXNnKTtcbn07XG5cbkFoZXVpSklULmNhbGxiYWNrcy5wcmludE51bSA9IG51bSA9PiB7IFxuICBzdGRpby5hZGRNc2cobnVtLnRvU3RyaW5nKCkpO1xufTtcblxuQWhldWlKSVQuY2FsbGJhY2tzLmlucHV0TnVtID0gKCkgPT4geyBcbiAgcmV0dXJuIHN0ZGluLnJlYWROdW0oKTtcbn07XG5cbkFoZXVpSklULmNhbGxiYWNrcy5pbnB1dENoYXIgPSAoKSA9PiB7IFxuICByZXR1cm4gc3RkaW4ucmVhZENoYXIoKTtcbn07XG5cbmZ1bmN0aW9uIHVzZUlucHV0KHsgY2xhc3NOYW1lLCBkZWZhdWx0VmFsdWUgfSkge1xuICBjb25zdCBbdmFsdWUsIHNldFZhbHVlXSA9IHVzZVN0YXRlKGRlZmF1bHRWYWx1ZSk7XG4gIGNvbnN0IGlucHV0ID0gPHRleHRhcmVhIGNsYXNzTmFtZT17Y2xhc3NOYW1lfSB2YWx1ZT17dmFsdWV9IG9uQ2hhbmdlPXtlID0+IHNldFZhbHVlKGUudGFyZ2V0LnZhbHVlKX0gIC8+O1xuICByZXR1cm4gW3ZhbHVlLCBpbnB1dCwgc2V0VmFsdWVdO1xufVxuXG5mdW5jdGlvbiBDb2RlUnVubmVyKCkge1xuICBjb25zdCBbcnVubmluZywgc2V0UnVubmluZ10gPSB1c2VTdGF0ZShmYWxzZSk7XG4gIGNvbnN0IFtpc1JlYWR5LCBzZXRJc1JlYWR5XSA9IHVzZVN0YXRlKGZhbHNlKTtcbiAgY29uc3QgW291dCwgc2V0T3V0XSA9IHVzZVN0YXRlKCcnKTtcbiAgY29uc3QgW2NvZGUsIGNvZGVJbnB1dCwgc2V0Q29kZUlucHV0XSA9IHVzZUlucHV0KHsgY2xhc3NOYW1lOiBcIkNvZGVcIiwgZGVmYXVsdFZhbHVlOiBcIuuwpOuwo+uUsOu5oOuwo+uwn+uUsOu/jFxcbuu5oOuno+2MjOu5qOuwm+uwpOuanOutj1xcbuuPi+uwrO2Dleu5oOuno+u2j+uRkOu2h1xcbuuzu+urg+uwleuwnOuat+2IrOutj+u2llxcbuurg+uPhOurg+2drOupk+utj+utj+u2mFxcbuurg+u0jO2GoOuylOuNlOuyjOu/jOuanFxcbuu9keu9gOupk+upk+uNlOuyk+u7kOuaoFxcbuu9gOuNqeuykOupk+u7kOuNleuNlOuyhVwiIH0pO1xuICBjb25zdCBbaW5wdXQsIGlucHV0SW5wdXQsIHNldElucHV0XSA9IHVzZUlucHV0KHsgY2xhc3NOYW1lOiBcIklucHV0XCIsIGRlZmF1bHRWYWx1ZTogXCJcIiB9KTtcbiAgdXNlRWZmZWN0KCgpID0+IHtcbiAgICBBaGV1aUpJVC5yZWFkeS50aGVuKCgpID0+IHtcbiAgICAgIHNldElzUmVhZHkodHJ1ZSk7XG4gICAgfSk7XG4gIH0sW10pO1xuXG4gIHVzZUVmZmVjdCgoKSA9PiB7XG4gICAgc3RkaW8ub25GbHVzaCA9IHggPT4geyBzZXRPdXQoeCk7IH07XG4gIH0sW10pO1xuXG4gIHJldHVybiAoXG4gICAgPD5cbiAgICAgIDxkaXYgY2xhc3NOYW1lPVwiUmVhZHlcIj4ocmVhZHk6IHtpc1JlYWR5ID8gXCJ5ZXNcIiA6IFwibm9cIn0pPC9kaXY+XG4gICAgICA8ZGl2PlxuICAgICAgICA8RXhhbXBsZXMgc2V0Q29kZT17KGNvZGUpID0+IHsgc2V0Q29kZUlucHV0KGNvZGUpO319IHNldElucHV0PXsoaSkgPT4geyBzZXRJbnB1dChpKTsgfX0vPlxuICAgICAgPC9kaXY+XG4gICAgICA8ZGl2IGNsYXNzTmFtZT1cIkNvbnRhaW5lclwiPlxuICAgICAgICA8ZGl2PlxuICAgICAgICAgIDxkaXY+aW5wdXQ8L2Rpdj5cbiAgICAgICAgICB7aW5wdXRJbnB1dH1cbiAgICAgICAgPC9kaXY+XG4gICAgICAgIDxkaXY+XG4gICAgICAgICAgPGRpdj5jb2RlPC9kaXY+XG4gICAgICAgICAge2NvZGVJbnB1dH1cbiAgICAgICAgPC9kaXY+XG4gICAgICA8L2Rpdj5cbiAgICAgIDxkaXY+XG4gICAgICB7IXJ1bm5pbmcgJiYgKDxkaXYgY2xhc3NOYW1lPVwiUnVuQnV0dG9uXCIgb25DbGljaz17KCkgPT4geyBcbiAgICAgICAgaWYgKCFpc1JlYWR5IHx8IGNvZGUubGVuZ3RoID09PSAwKVxuICAgICAgICAgIHJldHVybjtcbiAgICAgICAgc2V0UnVubmluZyh0cnVlKTtcbiAgICAgICAgc3RkaW4uc2V0QnVmZmVyKGlucHV0KTtcbiAgICAgICAgQWhldWlKSVQucnVuKGNvZGUpLnRoZW4oKCkgPT4ge1xuICAgICAgICAgIHN0ZGlvLmZsdXNoKCk7XG4gICAgICAgIH0pO1xuICAgICAgIH19PlJVTjwvZGl2PlxuICAgICAgKX1cbiAgICAgICA8L2Rpdj5cbiAgICAgIDxkaXYgY2xhc3NOYW1lPVwiT3V0XCI+e291dH08L2Rpdj5cblxuICAgICAgIDxDb25maWcvPlxuICAgIDwvPlxuICApO1xufVxuXG5leHBvcnQgZGVmYXVsdCBDb2RlUnVubmVyOyJdLCJzb3VyY2VSb290IjoiIn0=\n//# sourceURL=webpack-internal:///./components/CodeRunner.js\n");

/***/ })

})