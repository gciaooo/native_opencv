import 'dart:async';
import 'dart:ffi';
import 'dart:io';

import 'package:ffi/ffi.dart';
import 'package:flutter/services.dart';
import 'native_opencv_platform_interface.dart';

// Load our C lib
final DynamicLibrary nativeLib = Platform.isAndroid
    ? DynamicLibrary.open("libnative_opencv.so")
    : DynamicLibrary.process();

// C Function signatures
typedef CVersion = Pointer<Utf8> Function();
// Dart functions signatures
typedef DartVersion = Pointer<Utf8> Function();

// C function: cropDocument(char* input_path, char* query_path, char* output_path)
typedef CCropDocument = Void Function(
  Pointer<Utf8> input,
  Pointer<Utf8> query,
  Pointer<Utf8> output,
);
typedef DartCropDocument = void Function(
  Pointer<Utf8> input,
  Pointer<Utf8> query,
  Pointer<Utf8> output,
);

// Create dart functions that invoke the C function
final _version = nativeLib.lookupFunction<CVersion, DartVersion>('version');
final _cropDocument =
    nativeLib.lookupFunction<CCropDocument, DartCropDocument>('cropDocument');

class NativeOpencv {
  static const MethodChannel _channel = MethodChannel('native_opencv');

  static Future<String?> get platformVersion async {
    final String? version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }

  /// Crops document from filepath [input] based on [query] pdf image, and writes the result in [output]
  void cropDocument(String input, String query, String output) {
    return _cropDocument(
        input.toNativeUtf8(), query.toNativeUtf8(), output.toNativeUtf8());
  }

  String cvVersion() {
    return _version().toDartString();
  }

  Future<String?> getPlatformVersion() {
    return NativeOpencvPlatform.instance.getPlatformVersion();
  }
}
