import numpy
from numpy import zeros, sqrt
from numba import float64, struct, jit, autojit

_jacobian=struct([('row0',float64),
                  ('col0',float64),
                  ('dudrow',float64),
                  ('dudcol',float64),
                  ('dvdrow',float64),
                  ('dvdcol',float64),
                  ('det',float64),
                  ('sdet',float64)],packed=True)
_jacobian_dtype=_jacobian.get_dtype()


class Jacobian(object):
    def __init__(self, row0, col0, dudrow, dudcol, dvdrow, dvdcol):
        self._data = zeros(1, dtype=_jacobian_dtype)
        self.row0=row0
        self.col0=col0
        self._data['row0']=row0
        self._data['col0']=col0

        self._data['dudrow']=dudrow
        self._data['dudcol']=dudcol

        self._data['dvdrow']=dvdrow
        self._data['dvdcol']=dvdcol

        self._data['det'] = numpy.abs( dudrow*dvdcol-dudcol*dvdrow )
        self._data['sdet'] = sqrt(self._data['det'])

    def get_cen(self):
        """
        Get the center of the coordinate system
        """
        return self.row0, self.col0

    def set_cen(self, row0, col0):
        """
        reset the center
        """
        self._data['row0'] = row0
        self._data['col0'] = col0

    def get_det(self):
        """
        Get the determinant of the jacobian matrix
        """
        return self._data['det'][0]

    def get_scale(self):
        """
        Get the scale, defined as sqrt(det)
        """
        return self._data['sdet'][0]

    def copy(self):
        return Jacobian(self._data['row0'][0],
                        self._data['col0'][0],
                        self._data['dudrow'][0],
                        self._data['dudcol'][0],
                        self._data['dvdrow'][0],
                        self._data['dvdcol'][0])
    def __repr__(self):
        fmt="row0: %-10.5g col0: %-10.5g dudrow: %-10.5g dudcol: %-10.5g dvdrow: %-10.5g dvdcol: %-10.5g"
        return fmt % (self._data['row0'][0],
                      self._data['col0'][0],
                      self._data['dudrow'][0],
                      self._data['dudcol'][0],
                      self._data['dvdrow'][0],
                      self._data['dvdcol'][0])

class UnitJacobian(Jacobian):
    def __init__(self, cen1, cen2):
        super(UnitJacobian,self).__init__(cen1, cen2, 1., 0., 0., 1.)
