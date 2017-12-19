program read
     implicit none 
     include '/usr/include/netcdf.h'

     integer  ::  i,j,ncid,status
     integer  ::  ANOMid
   !  real*4     ::  x,y
     real     ::  ANOM(1388,584)


!!!!===============test nc read file=====================
     !status=nf_open('/home/liudi/fortran_test/SM_SCIE_MIR_CLF4DA_20101223T000000_20101223T235959_300_001_9.DBL.nc', nf_write, ncid)
     status=nc_open('/home/lincystar/nf90/SM_SCIE_MIR_CLF4DA_20101223T000000_20101223T235959_300_001_9.DBL.nc', NC_WRITE, ncid)
	 
     
     status=nf_close(ncid)
     if(status /= nf_NoErr) call handle_err(status)
    

end program read
subroutine handle_err(status)
  integer status
  print *, nf_strerror(status)
  stop 'stopped'
end subroutine
  


   
