if(WIN32)
    set(Macchina_DIR "C:/macchina")
    set(Macchina_BUNDLE_UTIL "${Macchina_DIR}/bin/bundle.exe")
else()
    set(Macchina_DIR "/usr/local/macchina")
    set(Macchina_BUNDLE_UTIL "${Macchina_DIR}/bin/bundle")
endif()

set(Macchina_INCLUDE_DIRS "${Macchina_DIR}/include")
set(Macchina_LIBRARY_DIRS "${Macchina_DIR}/lib")

