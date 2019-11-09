SHELL		=	/bin/sh
SRC_DIR		=	src
MAIN_DIR	=	main

HTSLIB_DIR	=	htslib

ABSTRACT_DIR	=	abstract
BIOSCIENCE_DIR	=	bioscience
MPI_DIR			=	mpi

include Makefile.config

CXX		:=	$(SRUN_PREFIX) $(CXX)
CXXFLAGS	+=	-I$(SRC_DIR) -I$(HTSLIB_DIR) $(OPENMP_CXXFLAGS)

SRC_SUFFIX	=	cpp
TEMPLATE_SUFFIX	=	hpp
HEADER_SUFFIX	=	hh

SUPP_CORE_SRC	=	$(wildcard $(SRC_DIR)/*.$(SRC_SUFFIX))
BAMEX_SRC	=	$(wildcard $(SRC_DIR)/$(MAIN_DIR)/bamex.$(SRC_SUFFIX))
BAMEX_SRC	+=	$(wildcard $(SRC_DIR)/$(BIOSCIENCE_DIR)/*.$(SRC_SUFFIX))
BAMEX_SRC	+=	$(SRC_DIR)/clicker.cpp
BAMEX_OBJ	=	$(BAMEX_SRC:.$(SRC_SUFFIX)=.o)
BAMEX_DEP	=	$(BAMEX_SRC:.$(SRC_SUFFIX)=.d)
-include $(BAMEX_DEP)

MPIIO_SRC	=	$(wildcard $(SRC_DIR)/$(MAIN_DIR)/mpiio.$(SRC_SUFFIX))
MPIIO_SRC	+=	$(wildcard $(SRC_DIR)/$(BIOSCIENCE_DIR)/*.$(SRC_SUFFIX))
MPIIO_SRC	+=	$(wildcard $(SRC_DIR)/$(MPI_DIR)/*.$(SRC_SUFFIX))
MPIIO_SRC	+=	$(SRC_DIR)/clicker.cpp
MPIIO_OBJ	=	$(MPIIO_SRC:.$(SRC_SUFFIX)=.o)
MPIIO_DEP	=	$(MPIIO_SRC:.$(SRC_SUFFIX)=.d)
-include $(MPIIO_DEP)

bamex: $(BAMEX_OBJ)
	$(CXX) $(OPENMP_CXXFLAGS) -o $@ $^ $(HTSLIB) -lboost_system -lboost_program_options -lomp

mpiio: $(MPIIO_OBJ)
	mpic++ $(OPENMP_CXXFLAGS) -o $@ $^ $(HTSLIB) -ltbb

etags:
	-@etags $(SRC_DIR)/*.$(SRC_SUFFIX) \
		$(SRC_DIR)/*.$(HEADER_SUFFIX) \
		$(SRC_DIR)/*.$(TEMPLATE_SUFFIX)

clean:
	-@$(RM) $(SRC_DIR)/*.o $(SRC_DIR)/*.d \
		$(SRC_DIR)/$(MAIN_DIR)/*.o $(SRC_DIR)/$(MAIN_DIR)/*.d \
		$(SRC_DIR)/$(ABSTRACT_DIR)/*.o $(SRC_DIR)/$(ABSTRACT_DIR)/*.d \
		$(SRC_DIR)/$(BIOSCIENCE_DIR)/*.o $(SRC_DIR)/$(BIOSCIENCE_DIR)/*.d \
		meta bamer exper bamex mpiio

distclean: clean
	-@$(RM) config.cache config.log config.status
	-@$(RM) TAGS
	-@$(RM) -rf autom4te.cache
