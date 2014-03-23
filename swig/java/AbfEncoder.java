/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.11
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package net.tdlsoftware.libabf;

public class AbfEncoder {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected AbfEncoder(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(AbfEncoder obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        libabfJNI.delete_AbfEncoder(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public AbfEncoder(String Filename) {
    this(libabfJNI.new_AbfEncoder__SWIG_0(Filename), true);
  }

  public AbfEncoder() {
    this(libabfJNI.new_AbfEncoder__SWIG_1(), true);
  }

  public void Initialize(String arg0) {
    libabfJNI.AbfEncoder_Initialize(swigCPtr, this, arg0);
  }

  public void SetTitle(String Title) {
    libabfJNI.AbfEncoder_SetTitle(swigCPtr, this, Title);
  }

  public void SetAuthor(String Author) {
    libabfJNI.AbfEncoder_SetAuthor(swigCPtr, this, Author);
  }

  public void SetTime(String Time) {
    libabfJNI.AbfEncoder_SetTime(swigCPtr, this, Time);
  }

  public void SetNumSections(int NumSections) {
    libabfJNI.AbfEncoder_SetNumSections(swigCPtr, this, NumSections);
  }

  public void WriteHeader() {
    libabfJNI.AbfEncoder_WriteHeader(swigCPtr, this);
  }

  public void WriteSection() {
    libabfJNI.AbfEncoder_WriteSection(swigCPtr, this);
  }

  public void Encode(SWIGTYPE_p_short Input) {
    libabfJNI.AbfEncoder_Encode(swigCPtr, this, SWIGTYPE_p_short.getCPtr(Input));
  }

}
