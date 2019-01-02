
    /*
    --------------------------------------------------------
     * RDEL-MESH-2: restricted delaunay mesh-gen. in R^2. 
    --------------------------------------------------------
     *
     * This program may be freely redistributed under the 
     * condition that the copyright notices (including this 
     * entire header) are not removed, and no compensation 
     * is received through use of the software.  Private, 
     * research, and institutional use is free.  You may 
     * distribute modified versions of this code UNDER THE 
     * CONDITION THAT THIS CODE AND ANY MODIFICATIONS MADE 
     * TO IT IN THE SAME FILE REMAIN UNDER COPYRIGHT OF THE 
     * ORIGINAL AUTHOR, BOTH SOURCE AND OBJECT CODE ARE 
     * MADE FREELY AVAILABLE WITHOUT CHARGE, AND CLEAR 
     * NOTICE IS GIVEN OF THE MODIFICATIONS.  Distribution 
     * of this code as part of a commercial system is 
     * permissible ONLY BY DIRECT ARRANGEMENT WITH THE 
     * AUTHOR.  (If you are not directly supplying this 
     * code to a customer, and you are instead telling them 
     * how they can obtain it for free, then you are not 
     * required to make any arrangement with me.) 
     *
     * Disclaimer:  Neither I nor: Columbia University, The
     * Massachusetts Institute of Technology, The 
     * University of Sydney, nor The National Aeronautics
     * and Space Administration warrant this code in any 
     * way whatsoever.  This code is provided "as-is" to be 
     * used at your own risk.
     *
    --------------------------------------------------------
     *
     * Last updated: 01 January, 2019
     *
     * Copyright 2013-2019
     * Darren Engwirda
     * de2363@columbia.edu
     * https://github.com/dengwirda/
     *
    --------------------------------------------------------
     */

#   pragma once

#   ifndef __RDEL_MESH_2__
#   define __RDEL_MESH_2__

    namespace mesh {
    
    template <
    typename M ,
    typename P ,
    typename G ,
    typename H ,
    typename A = allocators::basic_alloc
             >
    class rdel_mesh_2d
    { 
    public  : 
    
    /*-------- restricted delaunay mesh-generation in R^2 */  
    
    typedef M                               mesh_type ;
    typedef P                               mesh_pred ;
    typedef G                               geom_type ;
    typedef H                               hfun_type ;
    typedef A                               allocator ;

    typedef typename 
            mesh_type::real_type            real_type ;
    typedef typename 
            mesh_type::iptr_type            iptr_type ;
            
    typedef typename 
            allocator::size_type            uint_type ;

    typedef char_type  mode_type ;
    
    char_type static constexpr null_mode = +0 ;
    char_type static constexpr node_mode = +1 ;
    char_type static constexpr edge_mode = +2 ;
    char_type static constexpr etop_mode = +3 ;
    char_type static constexpr tria_mode = +4 ;

    class node_pred ; class ball_pred ; 
    class edge_pred ; class tria_pred ;
    
    class edge_cost : public mesh_pred::edge_data
        {
        public  :
        iptr_type           _node[ 2] ;
        iptr_type           _pass;
        } ;
    class tria_cost : public mesh_pred::tria_data
        {
        public  :
        iptr_type           _node[ 3] ;
        iptr_type           _pass;
        } ;
    
    typedef typename 
            mesh_type::node_data            node_data ;
    typedef typename 
            mesh_type::ball_data            ball_data ;
    typedef typename 
            mesh_type::edge_data            edge_data ;
    typedef typename 
            mesh_type::tria_data            tria_data ;

    typedef mesh::rdel_params       <
                real_type, 
                iptr_type           >       rdel_opts ;

    typedef containers::array       <
                iptr_type           >       iptr_list ;

/*------------------------------------------ cavity lists */
    typedef containers::array       <
                edge_data           >       edat_list ;
    
    typedef containers::array       <
                edge_cost           >       escr_list ;
    
    typedef containers::array       <
                tria_data           >       tdat_list ;
    
    typedef containers::array       <
                tria_cost           >       tscr_list ;

/*------------------------------------------ refine lists */
    typedef containers::priorityset <
                node_data, 
                node_pred           >       node_heap ;

    typedef containers::priorityset <
                edge_cost, 
                edge_pred           >       edge_heap ;

    typedef containers::priorityset <
                tria_cost, 
                tria_pred           >       tria_heap ;
                
/*------------------------------------------ collar lists */
    typedef containers::array       <
                ball_data           >       ball_list ;
                
    typedef containers::priorityset <
                ball_data, 
                ball_pred           >       ball_heap ;
                                           
    class node_pred
        {
    /*---------------------- "less-than" for node objects */
        public  :
        __inline_call bool_type operator() (
            node_data const& _idat,
            node_data const& _jdat
            ) const
        {   return _idat._pass < _jdat._pass ; 
        }
        } ;
    class ball_pred
        {
    /*---------------------- "less-than" for ball objects */
        public  :
        __inline_call bool_type operator() (
            ball_data const& _idat,
            ball_data const& _jdat
            ) const
        {   return _idat._ball[2] > 
                   _jdat._ball[2] ; 
        }
        } ;
    class edge_pred
        {
    /*---------------------- "less-than" for edge objects */
        public  :
        __inline_call bool_type operator() (
            edge_cost const& _idat,
            edge_cost const& _jdat
            ) const
        {   return mesh_pred
                 ::edge_pred(_idat, _jdat) ;
        }
        } ;
    class tria_pred
        {
    /*---------------------- "less-than" for tria objects */
        public  :
        __inline_call bool_type operator() (
            tria_cost const& _idat,
            tria_cost const& _jdat
            ) const
        {   return mesh_pred
                 ::tria_pred(_idat, _jdat) ;
        }
        } ;


    /*
    --------------------------------------------------------
     * RDEL-UPDATE: update the restricted-tria. 
    --------------------------------------------------------
     */
     
    #include "rdel_update_face_2.inc"
    
    
    /*
    --------------------------------------------------------
     * RDEL-BOUNDS: test restricted boundaries. 
    --------------------------------------------------------
     */
     
    #include "rdel_test_bounds_2.inc"
    
    
    /*
    --------------------------------------------------------
     * RDEL-REFINE: refine restricted subfaces. 
    --------------------------------------------------------
     */
     
    #include "rdel_refine_base_2.inc"
    #include "rdel_refine_ball_2.inc"
    #include "rdel_refine_face_2.inc"
    #include "rdel_refine_topo_2.inc"
    
    
    /*
    --------------------------------------------------------
     * TRIM-LIST: prune null faces from queues. 
    --------------------------------------------------------
     */
    
    template <
    typename      list_type
             >
    __static_call
    __inline_call void_type trim_list (
        list_type &_list
        )
    {
        typedef typename 
            list_type::
                size_type size_type;
        
        size_type _amin = +512;
        size_type _alim = +256;
        size_type _amax =
       (size_type)+3 * _list.count() ;
        size_type _anew =
       (size_type)+2 * _list.count() ;

        _anew = 
            std::max(_alim, _anew) ;

        if (_list.alloc() > _amin)
        if (_list.alloc() > _amax)
            _list.set_alloc(_anew) ;
    }

    /*
    --------------------------------------------------------
     * TRIM-EEPQ: prune null edges from queues. 
    --------------------------------------------------------
     */

    __static_call
    __normal_call void_type trim_eepq (
        mesh_type &_mesh,
        edge_heap &_eepq
        )
    {
        if(!_eepq.empty())
        {

        iptr_type _dead = +0 ;
        iptr_type _okay = +0 ;

        for (auto _hpos = _eepq.count() - 1 ; 
                  _hpos > +0 ; 
                --_hpos )
        {
            if (_okay > +1024 && 
                    _dead < _okay / 4) break;

            iptr_type _pass;
            edge_data _edat;
            _edat._node[0] = 
            _eepq. peek(_hpos)._node[0];
            _edat._node[1] = 
            _eepq. peek(_hpos)._node[1];

            _pass = 
            _eepq. peek(_hpos)._pass ;

            typename mesh_type::
                     edge_list::
                 item_type *_eptr = nullptr ;
            if(!_mesh.find_edge(_edat,_eptr))
            {
                _eepq._pop(_hpos) ;

                _dead += +1 ;
            }
            else
            if (_eptr->_data._pass != _pass )
            {
                _eepq._pop(_hpos) ;

                _dead += +1 ;
            }
            else
            {
                _okay += +1 ;
            }
        }
        }
        
        trim_list ( _eepq ) ;
    }

    /*
    --------------------------------------------------------
     * TRIM-TTPQ: prune null tria. from queues. 
    --------------------------------------------------------
     */

    __static_call
    __normal_call void_type trim_ttpq (
        mesh_type &_mesh,
        tria_heap &_ttpq
        )
    {
        if(!_ttpq.empty())
        {

        iptr_type _dead = +0 ;
        iptr_type _okay = +0 ;

        for (auto _hpos = _ttpq.count() - 1 ; 
                  _hpos > +0 ; 
                --_hpos )
        {
            if (_okay > +1024 && 
                    _dead < _okay / 4) break;

            iptr_type _pass;
            tria_data _tdat;
            _tdat._node[0] = 
            _ttpq. peek(_hpos)._node[0];
            _tdat._node[1] = 
            _ttpq. peek(_hpos)._node[1];
            _tdat._node[2] = 
            _ttpq. peek(_hpos)._node[2];

            _pass = 
            _ttpq. peek(_hpos)._pass ;

            typename mesh_type::
                     tria_list::
                 item_type *_tptr = nullptr ;
            if(!_mesh.find_tria(_tdat,_tptr))
            {
                _ttpq._pop(_hpos);

                _dead += +1 ;
            }
            else
            if (_tptr->_data._pass != _pass )
            {
                _ttpq._pop(_hpos);

                _dead += +1 ;
            }
            else
            {
                _okay += +1 ;
            }
        }
        }
        
        trim_list ( _ttpq ) ;
    }

    /*
    --------------------------------------------------------
     * INIT-RDEL: init. face-set in rDT. 
    --------------------------------------------------------
     */

    __static_call 
    __normal_call void_type init_rdel (
        geom_type &_geom ,
        hfun_type &_hfun ,
        mesh_type &_mesh ,
        iptr_list &_nnew ,
        iptr_list &_tnew ,
        edat_list &_ecav ,
        escr_list &_escr ,
        tdat_list &_tcav ,
        tscr_list &_tscr ,
        ball_list &_bcav ,
        ball_list &_bscr ,
        iptr_type  _pass ,
        mode_type  _fdim ,
        rdel_opts &_args
        )
    {
    /*-------------------- mark all existing elem. as new */
        iptr_type _npos = 0, _tpos = 0 ;
        for (auto _iter  = 
            _mesh._tria._nset.head() ; 
                  _iter != 
            _mesh._tria._nset.tend() ; 
                ++_iter  , ++_npos)
        {
            if (_iter->mark() >= +0)
            {
            _nnew. push_tail( _npos) ;
            }
        }
        for (auto _iter  = 
            _mesh._tria._tset.head() ; 
                  _iter != 
            _mesh._tria._tset.tend() ; 
                ++_iter  , ++_tpos)
        {
            if (_iter->mark() >= +0)
            {
            _tnew. push_tail( _tpos) ;
            }
        }
    /*-------------------- init. restricted triangulation */
        push_rdel( _geom, _hfun, 
            _mesh, _nnew, _tnew, 
            _escr, _ecav, 
            _tscr, _tcav,
            _bscr, _bcav,    
               -1, _pass,
            _fdim, _fdim, _args) ;
    }

    /*
    --------------------------------------------------------
     * INIT-MESH: init. the bounding DT. 
    --------------------------------------------------------
     */

    template <
    typename      init_type
             >
    __static_call
    __normal_call void_type init_sort (
        init_type &_init,
        iptr_list &_iset
        )
    {
        typedef geom_tree::aabb_node_base_k     
                           tree_node ;

        typedef geom_tree::aabb_item_node_k <
            real_type,
            iptr_type, 2>  tree_item ;
                    
        typedef geom_tree::aabb_tree <
            tree_item, 2,
            tree_node,
            allocator   >  tree_type ;
                
        containers::array<tree_item> _bbox;
       
    /*------------------------------ initialise aabb-tree */ 
        iptr_type _npos  = 0 ;
        tree_type _tree  ;
        for (auto _node  = 
            _init._mesh._set1.head() ; 
                  _node != 
            _init._mesh._set1.tend() ;
                ++_node, ++_npos)
        {
            if (_node->mark() >= +0)
            {
            
            _bbox.push_tail() ;
            _bbox.tail()->
                pval(0) = _node->pval( 0) ;
            _bbox.tail()->
                pval(1) = _node->pval( 1) ;
  
            _bbox.tail()->
                ipos () = _npos ;
            
            }
        }
       
        iptr_type constexpr _NBOX = +64 ;
       
        _tree.load(_bbox.head(),
                   _bbox.tend(), _NBOX) ;
        
    /*------------------------------ randomised tree sort */    
        _tree.brio(_iset) ;
    }

    template <
    typename      init_type
             >
    __static_call
    __normal_call void_type init_mesh (
        geom_type &_geom,
        init_type &_init,
        hfun_type &_hfun,
        mesh_type &_mesh,
        rdel_opts &_opts
        )
    {
        __unreferenced(_hfun) ;

    /*------------------------------ initialise mesh bbox */
        real_type _plen[ +2];
        _plen[ 0] = _geom._bmax[ 0] - 
                    _geom._bmin[ 0] ;
        _plen[ 1] = _geom._bmax[ 1] - 
                    _geom._bmin[ 1] ;
        
        _plen[ 0]*= (real_type)+2.0 ;
        _plen[ 1]*= (real_type)+2.0 ;
        
        real_type _pmin[ +2];
        real_type _pmax[ +2];
        _pmin[ 0] = _geom._bmin[ 0] - 
                          _plen[ 0] ;
        _pmin[ 1] = _geom._bmin[ 1] - 
                          _plen[ 1] ;
        
        _pmax[ 0] = _geom._bmax[ 0] + 
                          _plen[ 0] ;
        _pmax[ 1] = _geom._bmax[ 1] + 
                          _plen[ 1] ;
        
        _mesh.
        _tria.push_root(_pmin, _pmax) ;

    /*------------------------------ initialise mesh root */
        _mesh.
        _tria.node(+0)->fdim() = +3 ;
        _mesh.
        _tria.node(+1)->fdim() = +3 ;
        _mesh.
        _tria.node(+2)->fdim() = +3 ;
        
        _mesh.
        _tria.node(+0)->feat() = +0 ;
        _mesh.
        _tria.node(+1)->feat() = +0 ;
        _mesh.
        _tria.node(+2)->feat() = +0 ;
        
        _mesh.
        _tria.node(+0)->topo() = +0 ;
        _mesh.
        _tria.node(+1)->topo() = +0 ;
        _mesh.
        _tria.node(+2)->topo() = +0 ;
    
    /*------------------------------ seed feat from geom. */
        _geom.seed_feat(_mesh, _opts) ;
                        
    /*------------------------------ seed mesh from init. */
        iptr_type _hint  = -1;
        iptr_list _iset  ;
        init_sort(_init,_iset) ;
        for (auto _iter  = _iset.head();
                  _iter != _iset.tend();
                ++_iter  )
        {
            auto _node = &_init.
                _mesh._set1[*_iter] ;
        
            iptr_type _npos = -1 ;
            if (_mesh._tria.push_node (
               &_node->pval(0) , 
                _npos, _hint ) )
            {
            
            _mesh._tria.node
                (_npos)->fdim() 
                    = _node->fdim() ;
                        
            _mesh._tria.node
                (_npos)->feat() 
                    = _node->feat() ;
                    
            _mesh._tria.node
                (_npos)->topo() = 2 ;  
            
            _hint = _mesh._tria.
                node(_npos)->next() ;
            
            }
        }
        
    /*------------------------------ seed mesh from geom. */
        _geom.seed_mesh(_mesh, _opts) ;
            
    }

    /*
    --------------------------------------------------------
     * RDEL-MESH: build an rDT mesh in R^2. 
    --------------------------------------------------------
     */

    template <
    typename      init_type ,
    typename      jlog_file
             >
    __static_call
    __normal_call void_type rdel_mesh (
        geom_type &_geom ,
        init_type &_init ,
        hfun_type &_hfun ,
        mesh_type &_mesh ,
        rdel_opts &_args ,
        jlog_file &_dump
        )
    {   
        mode_type _mode = null_mode ;
        
    /*------------------------------ push log-file header */
        _dump.push (
    "#------------------------------------------------------------\n"
    "#    |ITER.|      |DEL-1|      |DEL-2| \n"
    "#------------------------------------------------------------\n"
            ) ;

    /*------------------------------ ensure deterministic */  
        std::srand( +1 ) ;

    /*------------------------------ init. list workspace */
        iptr_list _nnew, _nold ;
        iptr_list _tnew, _told ;
        
        escr_list _escr  ;
        tscr_list _tscr  ;
        ball_list _bscr  ;
        
        edat_list _edat, _etmp ;
        tdat_list _tdat  ;
        ball_list _bdat  ;

    /*------------------------------ refinement p.-queues */
        edge_heap _eepq  ;
        tria_heap _ttpq  ;
        node_heap _etpq  ;
        ball_heap _nbpq  ;
 
    /*------------------------------ alloc. for hash obj. */
        _mesh._eset._lptr. set_count (
        _mesh._tria._tset.count()*+3 , 
        containers::loose_alloc, nullptr);
        _mesh._tset._lptr. set_count (
        _mesh._tria._tset.count()*+1 , 
        containers::loose_alloc, nullptr);

    /*------------------------------ init. topo hash obj. */
        typename 
            mesh_type::node_list _etin (
        typename mesh_type::node_hash(),
        typename mesh_type::node_pred(), 
            +.8,_mesh._nset.get_alloc()) ;
        
        typename 
            mesh_type::edge_list _pedg (
        typename mesh_type::edge_hash(),
        typename mesh_type::edge_pred(), 
            +.8,_mesh._eset.get_alloc()) ;
 
    /*------------------------------ init. point counters */
        containers:: fixed_array<
            iptr_type ,
            rdel_opts::last_kind> _enod;          
        containers:: fixed_array<
            iptr_type , 
            rdel_opts::last_kind> _tnod;

        _enod.fill( +0 ) ;
        _tnod.fill( +0 ) ;

    /*------------------------------ initialise mesh obj. */
        init_mesh( _geom , _init, _hfun, 
            _mesh, _args ) ;
        
    /*-------------------- calc. size-func for seed nodes */ 
       
        for (auto _node  = 
            _mesh._tria._nset.head(); 
                  _node != 
            _mesh._tria._nset.tend();
                ++_node  )
        {
            if (_node->mark() >= +0)
            {
                _node->idxh()  = 
                    hfun_type::null_hint();
            }
        }

    /*-------------------- main: refine edges/faces/trias */
    
        iptr_type _pass = +0   ;
    
        for(bool_type _done=false; !_done ; )
        {
            iptr_type _trim_freq = +10000 ;
    #       ifdef _DEBUG
            iptr_type _jlog_freq = +250 ;
    #       else
            iptr_type _jlog_freq = +50000 ;
    #       endif

            if(++_pass>_args.iter()) break;
        
        /*------------------------- init. array workspace */
            _nnew.set_count(  +0 ) ;
            _nold.set_count(  +0 ) ;
            _tnew.set_count(  +0 ) ;
            _told.set_count(  +0 ) ;
            
            _escr.set_count(  +0 ) ;
            _tscr.set_count(  +0 ) ;
            _bscr.set_count(  +0 ) ;
            
            _etmp.set_count(  +0 ) ;
            _edat.set_count(  +0 ) ;
            _tdat.set_count(  +0 ) ;
            _bdat.set_count(  +0 ) ;
            
        /*--------- calc. "restricted-ness" incrementally */

            if (_mode == null_mode )
            {
        /*------------------------- init. protecting ball */
                _mode  = node_mode ;
             
                init_rdel( _geom, _hfun, 
                    _mesh, _nnew, _tnew, 
                    _edat, _escr, 
                    _tdat, _tscr,
                    _bdat, _bscr, _pass, 
                    _mode, _args)  ;
            }
       
            if (_mode == node_mode &&
                    _nbpq. empty() &&
                    _bscr. empty() &&
                    _bdat. empty() )
            {
        /*------------------------- init. restricted edge */
                _mode  = edge_mode ;
               
                init_rdel( _geom, _hfun, 
                    _mesh, _nnew, _tnew, 
                    _edat, _escr, 
                    _tdat, _tscr,
                    _bdat, _bscr, _pass, 
                    _mode, _args)  ;
            }          
            if (_mode == edge_mode && 
                    _eepq. empty() &&
                    _escr. empty() &&
                    _edat. empty() )
            {
        /*------------------------- init. restricted topo */
                _mode  = etop_mode ;
            }
                  
            if (_mode == etop_mode && 
                    _etpq. empty() &&
                    _escr. empty() &&
                    _edat. empty() )
            {
        /*------------------------- init. restricted tria */
                _mode  = tria_mode ;
        
                init_rdel( _geom, _hfun, 
                    _mesh, _nnew, _tnew, 
                    _edat, _escr, 
                    _tdat, _tscr,
                    _bdat, _bscr, _pass, 
                    _mode, _args)  ;
            }

        /*------------- refine "bad" sub-faces until done */

            if ( _bscr.empty() &&
                 _bdat.empty() &&
                 _escr.empty() &&
                 _edat.empty() &&
                 _tscr.empty() &&
                 _tdat.empty() )
            {

            char_type _tdim = -1;
            typename rdel_opts::node_kind 
            _kind =  rdel_opts::null_kind ;
            
            if (!_nbpq.empty() )
            {
        /*----------------------------- refine "bad" ball */
                _kind =_bad_ball( _geom,
                    _hfun, _mesh, _mode,
                    _pedg, _nnew, _nold,
                    _tnew, _told, _nbpq,
                    _etmp, _edat, _escr,
                    _tdat, _tscr, 
                    _bdat, _bscr, _tdim, 
                    _pass, _args)  ;
            }
            else
            if (!_eepq.empty() )
            {
        /*----------------------------- refine "bad" edge */
                _kind =_bad_edge( _geom, 
                    _hfun, _mesh, _mode,
                    _pedg, _nnew, _nold, 
                    _tnew, _told, _eepq,
                    _etmp, _edat, _escr, 
                    _tdat, _tscr, 
                    _bdat, _bscr, _tdim, 
                    _pass, _args)  ;
            }
            else
            if (!_etpq.empty() )
            {
        /*----------------------------- refine "bad" topo */
                _kind =_bad_etop( _geom, 
                    _hfun, _mesh, _mode, 
                    _pedg, _nnew, _nold, 
                    _tnew, _told,
                    _etpq, _etin, 
                    _etmp, _edat, _escr, 
                    _tdat, _tscr, 
                    _bdat, _bscr, _tdim, 
                    _pass, _args)  ;
            }
            else
            if (!_ttpq.empty() )
            {
        /*----------------------------- refine "bad" tria */
                _kind =_bad_tria( _geom, 
                    _hfun, _mesh, _mode, 
                    _pedg, _nnew, _nold,
                    _tnew, _told, _ttpq, 
                    _etmp, _edat, _escr,
                    _tdat, _tscr, 
                    _bdat, _bscr, _tdim, 
                    _pass, _args)  ;
            }
        /*----------------------------- meshing converged */
            else { _done = true ;  }
    
            if (_pass%_jlog_freq==+0 || _done )
            {
        /*----------------------------- output to logfile */
                std::stringstream _sstr;
                _sstr << std::setw(11) <<
                    _pass
                      << std::setw(13) << 
                    _mesh._eset.count()
                      << std::setw(13) << 
                    _mesh._tset.count()
                      <<   "\n" ;
                _dump.push(_sstr.str());
            }

            if (_kind != rdel_opts::null_kind )
            {
        /*--------------- update point-placement counters */
                if (_tdim == +1)
                    _enod[_kind] += +1 ;
                else
                if (_tdim == +2)
                    _tnod[_kind] += +1 ;
            }

            }
            
            if (_pass%_trim_freq == +0 )
            {
        /*--------------- trim null PQ items "on-the-fly" */
              //trim_nbpq(_mesh, _nbpq);
                trim_eepq(_mesh, _eepq);
                trim_ttpq(_mesh, _ttpq);
              //trim_etpq(_mesh, _etpq);
              
                trim_list(_nnew) ;
                trim_list(_nold) ;
                trim_list(_tnew) ;
                trim_list(_told) ;
                
                trim_list(_etmp) ;
                trim_list(_edat) ;
                trim_list(_escr) ;
                trim_list(_tdat) ;
                trim_list(_tscr) ;
                trim_list(_bscr) ;
                trim_list(_bdat) ;
            }

        /*----------------------------- enqueue edge topo */ 
        
            for (auto _edge  = _edat.head();
                      _edge != _edat.tend();
                    ++_edge  )
            {
            /*------------------------- bypass "old" edge */
                typename mesh_type::
                         edge_list::
                     item_type *_eptr=nullptr;
                if (_mesh.
                     find_edge(*_edge, _eptr))
                    continue ;
                
            /*------------------------- enqueue edge node */                
                iptr_type _npos;
                for (_npos = +2; _npos-- != +0; )
                {        
                    iptr_type _node = 
                        _edge->_node[_npos] ;
                
                    node_data _ndat;
                    _ndat._pass    = _pass;
                    _ndat._node[0] = _node;
                    
                    typename mesh_type::
                             node_list::
                         item_type*_nptr=nullptr;
                    if(!_etin.find(_ndat, _nptr))
                    {
                        if (_args.top1())
                        {          
                /*--------------------- enqueue edge node */
                        _etin.push(_ndat) ;
                        _etpq.push(_ndat) ;
                        }
                    }
                }
            }
            
        /*--------------- update restricted triangulation */
            
            for (auto _npos  = _nold.head() ;
                      _npos != _nold.tend() ; 
                    ++_npos  )
            {
                ball_data  _ball, _same;
                _ball._node[0] = *_npos;
                _ball._kind = feat_ball;                
                _mesh.
                _pop_ball( _ball, _same) ;
            }
                    
            for (auto _tpos  = _told.head() ;
                      _tpos != _told.tend() ; 
                    ++_tpos  )
            {
                _pop_edge(_mesh, *_tpos) ;
                _pop_tria(_mesh, *_tpos) ;
            }
            for (auto _tpos  = _told.head() ;
                      _tpos != _told.tend() ; 
                    ++_tpos  )
            {
                _mesh.
                _tria._put_tria( *_tpos) ;
            }
            
            for (auto _iter  = _bscr.head() ;
                      _iter != _bscr.tend() ; 
                    ++_iter  )
            {
                _nbpq .push( *_iter ) ;
            }
            for (auto _iter  = _escr.head() ;
                      _iter != _escr.tend() ; 
                    ++_iter  )
            {
                _eepq .push( *_iter ) ;
            }
            for (auto _iter  = _tscr.head() ;
                      _iter != _tscr.tend() ; 
                    ++_iter  )
            {
                _ttpq .push( *_iter ) ;
            }
            
            for (auto _iter  = _bdat.head() ;
                      _iter != _bdat.tend() ; 
                    ++_iter  )
            {
                _mesh.push_ball( *_iter) ;
            }
            for (auto _iter  = _edat.head() ;
                      _iter != _edat.tend() ; 
                    ++_iter  )
            {
                _mesh.push_edge( *_iter) ;
            }       
            for (auto _iter  = _tdat.head() ;
                      _iter != _tdat.tend() ; 
                    ++_iter  )
            {
                _mesh.push_tria( *_iter) ;
            }
              
        }

        /*
        if (_args.verb() >= +2 )
        {
    //-------------------- push refinement memory metrics *
        
        _dump.push("\n")  ;
        _dump.push("  MEMORY statistics... \n") ;
        _dump.push("\n")  ;
        
        }
         */
        
        if (_args.verb() >= +2 )
        {
    /*-------------------- push refinement scheme metrics */
        
        _dump.push("\n")  ;
        _dump.push("  REFINE statistics... \n") ;
        _dump.push("\n")  ;

        _dump.push("  |TYPE-1| (edge) = ") ;
        _dump.push(std::to_string(
             _enod[rdel_opts::circ_kind])) ;
        _dump.push("\n")  ;
        _dump.push("  |TYPE-2| (edge) = ") ;
        _dump.push(std::to_string(
             _enod[rdel_opts::offh_kind])) ;
        _dump.push("\n")  ;
        _dump.push("  |TYPE-D| (edge) = ") ;
        _dump.push(std::to_string(
             _enod[rdel_opts::disk_kind])) ;
        _dump.push("\n")  ;
        _dump.push("\n")  ;

        _dump.push("  |TYPE-1| (tria) = ") ;
        _dump.push(std::to_string(
             _tnod[rdel_opts::circ_kind])) ;
        _dump.push("\n")  ;
        _dump.push("  |TYPE-2| (tria) = ") ;
        _dump.push(std::to_string(
             _tnod[rdel_opts::offh_kind])) ;
        _dump.push("\n")  ;
        _dump.push("  |TYPE-3| (tria) = ") ;
        _dump.push(std::to_string(
             _tnod[rdel_opts::offc_kind])) ;
        _dump.push("\n")  ;
        _dump.push("  |TYPE-4| (tria) = ") ;
        _dump.push(std::to_string(
             _tnod[rdel_opts::sink_kind])) ;
        _dump.push("\n")  ;
        
        }
        
        _dump.push("\n")  ;
    }
    
    } ;

 
    }
    
#   endif   //__RDEL_MESH_2__
    
    
    
