    
    /*
    --------------------------------------------------------
     * RDEL-MAKE-3: restricted delaunay tria. in R^3. 
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
     * Last updated: 29 December, 2018
     *
     * Copyright 2013-2018
     * Darren Engwirda
     * de2363@columbia.edu
     * https://github.com/dengwirda/
     *
    --------------------------------------------------------
     */

#   pragma once

#   ifndef __RDEL_MAKE_3__
#   define __RDEL_MAKE_3__

    namespace mesh {
    
    template <
    typename M ,
    typename G ,
    typename A = allocators::basic_alloc
             >
    class rdel_make_3d
    { 
    public  : 
    
    /*----------- restricted delaunay tessellation in R^3 */  
    
    typedef M                               mesh_type ;
    typedef G                               geom_type ;
    typedef A                               allocator ;

    typedef typename 
            mesh_type::real_type            real_type ;
    typedef typename 
            mesh_type::iptr_type            iptr_type ;
            
    typedef typename 
            allocator::size_type            uint_type ;

    typedef typename 
            mesh_type::node_data            node_data ;
    typedef typename 
            mesh_type::ball_data            ball_data ;
    typedef typename 
            mesh_type::edge_data            edge_data ;
    typedef typename 
            mesh_type::face_data            face_data ;
    typedef typename 
            mesh_type::tria_data            tria_data ;

    typedef containers::array       <
                iptr_type           >       iptr_list ;
    
    typedef mesh::rdel_pred_base_3  <
                geom_type,
                mesh_type           >       rdel_pred ;

    typedef mesh::rdel_params       <
                real_type, 
                iptr_type           >       rdel_opts ;
                
                
    /*
    --------------------------------------------------------
     * INIT-BALL: add new ball to restricted-tria.
    --------------------------------------------------------
     */
    
    __static_call
    __normal_call void_type init_ball (
        mesh_type &_mesh,
        geom_type &_geom,
        iptr_type  _npos,
        char_type  _kind,
        iptr_type &_nbal,
        rdel_opts &_opts
        )
    {
        __unreferenced ( _geom ) ;
        __unreferenced ( _opts ) ;
    
        if (_mesh._tria.
             node(_npos)->feat()==hard_feat)
        {
    /*---------- push protecting ball for "hard" features */
            ball_data _ball ;
            _ball._node[0] = _npos;
            
            _ball._pass    =   +0 ;
            _ball._kind    = _kind;
            
            _ball._ball[0] = _mesh.
                _tria.node(_npos)->pval(0) ;
            _ball._ball[1] = _mesh.
                _tria.node(_npos)->pval(1) ;
            _ball._ball[2] = _mesh.
                _tria.node(_npos)->pval(2) ;
            
            _ball._ball[3] = (real_type)0. ;
    
            _nbal +=   +1;
    
            _mesh.push_ball (_ball) ;        
        }
    }
    
    /*
    --------------------------------------------------------
     * PUSH-EDGE: add new edge to restricted-tria.
    --------------------------------------------------------
     */
    
    __static_call
    __normal_call void_type test_edge (
        mesh_type &_mesh ,
        geom_type &_geom ,
        iptr_type  _tpos ,
        typename 
    mesh_type::edge_list & _edge_test ,
        iptr_type &_nedg ,
        rdel_opts &_opts
        )
    {
    /*-------------------------------- check "restricted" */
        for (auto _fpos =+6; _fpos-- != +0; )
        {
        /*---------------------------- extract edge nodes */
            iptr_type _tnod[ +4] ;
            mesh_type::tria_type::
                tria_type::
            face_node(_tnod, _fpos, +3, +1) ;          
            _tnod[0] = _mesh._tria.
            tria(_tpos)->node(_tnod[0]);
            _tnod[1] = _mesh._tria.
            tria(_tpos)->node(_tnod[1]);

        /*--------------- face contains higher dim. nodes */
            if (_mesh._tria.node(
                _tnod[0])->fdim() > 1 ||
                _mesh._tria.node(
                _tnod[1])->fdim() > 1 )
                continue   ;
            
            algorithms::isort (
                &_tnod[0], &_tnod[2], 
                    std::less<iptr_type>()) ;

            edge_data _edat;
            _edat._node[0] = _tnod[ 0] ;
            _edat._node[1] = _tnod[ 1] ;

            typename mesh_type::
                     edge_list::
                item_type *_mptr = nullptr  ;
            if(_edge_test.
                find( _edat, _mptr) ) 
            {
        /*--------------------------- don't test repeats! */
                continue   ;
            }

            _edat._tadj    = _tpos;
            _edat._eadj    = _fpos;
            _edat._pass    =   +0 ;
            
        /*--------------------------- call edge predicate */
            char_type _hits;
            real_type _fbal[ 4];
            real_type _sbal[ 4];
            
            __unreferenced(_opts);
            
            bool_type _rBND  =
            rdel_pred::edge_ball (
                _geom,_mesh, 
                _edat._tadj,
                _edat._eadj,
                _fbal,_sbal,
                _hits,
                _edat._feat,
                _edat._topo,
                _edat._part) ;
            
        /*--------------------------- push edge onto mesh */
            if (_rBND) _nedg += +1 ;

            if (_rBND)
            _mesh.push_edge(_edat) ;
                
                
            _edge_test.push(_edat) ;

        } // for (auto _fpos = +6; _fpos-- != +0; )        
    }
    
    /*
    --------------------------------------------------------
     * PUSH-FACE: add new face to restricted-tria.
    --------------------------------------------------------
     */
    
    __static_call
    __normal_call void_type test_face (
        mesh_type &_mesh ,
        geom_type &_geom ,
        iptr_type  _tpos ,
        typename 
    mesh_type::face_list & _face_test ,
        iptr_type &_nfac ,
        iptr_type &_ndup ,
        rdel_opts &_opts
        )
    {
    /*-------------------------------- check "restricted" */
        for (auto _fpos =+4; _fpos-- != +0; )
        {
        /*---------------------------- extract face nodes */
            iptr_type _tnod[ +4] ;
            mesh_type::tria_type::
                tria_type::
            face_node(_tnod, _fpos, +3, +2) ;
            _tnod[0] = _mesh._tria.
            tria(_tpos)->node(_tnod[0]);
            _tnod[1] = _mesh._tria.
            tria(_tpos)->node(_tnod[1]);
            _tnod[2] = _mesh._tria.
            tria(_tpos)->node(_tnod[2]);

        /*--------------- face contains higher dim. nodes */
            if (_mesh._tria.node(
                _tnod[0])->fdim() > 2 ||
                _mesh._tria.node(
                _tnod[1])->fdim() > 2 ||
                _mesh._tria.node(
                _tnod[2])->fdim() > 2 )
                continue   ;

            algorithms::isort (
                &_tnod[0], &_tnod[3], 
                    std::less<iptr_type>()) ;

            face_data _fdat;
            _fdat._node[0] = _tnod[ 0] ;
            _fdat._node[1] = _tnod[ 1] ;
            _fdat._node[2] = _tnod[ 2] ;

            typename mesh_type::
                     face_list::
                item_type *_mptr = nullptr  ;
            if(_face_test.
                find( _fdat, _mptr) ) 
            {
        /*--------------------------- count bnd. repeats! */
                _ndup += 
                _mptr->_data._dups;
                
        /*--------------------------- don't test repeats! */
                continue   ;
            }

            _fdat._tadj    = _tpos;
            _fdat._fadj    = _fpos;
            _fdat._pass    = 0 ;
            _fdat._dups    = 0 ; // count num. dup's
                                 // only in hash-set
            
        /*--------------------------- call face predicate */
            char_type _feat, _topo;
            real_type _fbal[ 4];
            real_type _sbal[ 4];
            
            __unreferenced(_opts);
            
            bool_type _rBND    =
            rdel_pred::face_ball (
                _geom,_mesh, 
                _fdat._tadj,
                _fdat._fadj,
                _fbal,_sbal,
                _feat,_topo,
                _fdat._part)   ;
                
        /*--------------------------- push face onto mesh */
            if (_rBND) _nfac += +1 ;

            if (_rBND)
                _fdat._dups   = +1 ;

            if (_rBND)
            _mesh.push_face(_fdat) ;


            _face_test.push(_fdat) ;

        } // for (auto _fpos = +4; _fpos-- != +0; )
    }
    
    /*
    --------------------------------------------------------
     * PUSH-TRIA: add new tria to restricted-tria.
    --------------------------------------------------------
     */
    
    __static_call
    __normal_call void_type test_tria (
        mesh_type &_mesh ,
        geom_type &_geom ,
        iptr_type  _tpos ,
        iptr_type &_sign ,
        typename 
    mesh_type::tria_list & _tria_test ,
        iptr_type &_ntri ,
        rdel_opts &_opts
        )
    {
    /*-------------------------------- check "restricted" */
        {
            iptr_type  _tnod[ +4] ;
            _tnod[0] = _mesh.
            _tria.tria(_tpos)->node(0);
            _tnod[1] = _mesh.
            _tria.tria(_tpos)->node(1);
            _tnod[2] = _mesh.
            _tria.tria(_tpos)->node(2);
            _tnod[3] = _mesh.
            _tria.tria(_tpos)->node(3);

        /*--------------- face contains higher dim. nodes */
            if (_mesh._tria.node(
                _tnod[0])->fdim() > 3 ||
                _mesh._tria.node(
                _tnod[1])->fdim() > 3 ||
                _mesh._tria.node(
                _tnod[2])->fdim() > 3 ||
                _mesh._tria.node(
                _tnod[3])->fdim() > 3 )
                return ;

            tria_data _tdat;
            _tdat._node[0] = _tnod[ 0] ;
            _tdat._node[1] = _tnod[ 1] ;
            _tdat._node[2] = _tnod[ 2] ;
            _tdat._node[3] = _tnod[ 3] ;

            _tdat._tadj    = _tpos;

            typename mesh_type::
                     tria_list::
                item_type *_mptr = nullptr;
            if(_tria_test.
                find( _tdat, _mptr) )
            { 
        /*--------------------------- don't test repeats! */
                return ;
            }

        /*--------------------------- call tria predicate */
            _tdat._part =  _sign ;
            _tdat._pass =    +0  ;

            real_type _tbal[ +4] ;

            __unreferenced(_opts);

            bool_type _rBND   = 
            rdel_pred::tria_ball (
                _geom,_mesh,
                _tdat._tadj,
                _tbal,
                _tdat._part)  ;

            _sign = _tdat. _part ;

        /*--------------------------- push tria onto mesh */
            if (_rBND) _ntri += +1 ;

            if (_rBND)
            _mesh.push_tria(_tdat) ;
            
            
        //!!_tria_test.push(_tdat) ;  won't have repeats!
        
        }
    }
    
    /*
    --------------------------------------------------------
     * TRIA-CIRC: calc. circumball for tria.
    --------------------------------------------------------
     */

    __static_call
    __normal_call void_type tria_circ ( // get "exact" tbal?
        mesh_type &_mesh ,
        iptr_type  _tpos
        )
    {
        iptr_type _tnod[4] = {
        _mesh.
        _tria.tria( _tpos)->node(0) ,
        _mesh.
        _tria.tria( _tpos)->node(1) ,
        _mesh.
        _tria.tria( _tpos)->node(2) ,
        _mesh.
        _tria.tria( _tpos)->node(3)
            } ;

        algorithms::isort(
            &_tnod[0], &_tnod[4], 
                std::less<iptr_type>()) ;
    
    /*---------------------- calc. ball in floating-point */
        real_type _tbal[4] ;
        geometry::circ_ball_3d (
            _tbal , 
       &_mesh._tria.
            node(_tnod[0])->pval(0) , 
       &_mesh._tria.
            node(_tnod[1])->pval(0) ,
       &_mesh._tria.
            node(_tnod[2])->pval(0) ,
       &_mesh._tria.
            node(_tnod[3])->pval(0)
            ) ;
            
        _mesh._tria.tria(
        _tpos)->circ(0) =  _tbal[0] ;
        _mesh._tria.tria(
        _tpos)->circ(1) =  _tbal[1] ;
        _mesh._tria.tria(
        _tpos)->circ(2) =  _tbal[2] ;
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
            iptr_type, 3>  tree_item ;
                    
        typedef geom_tree::aabb_tree <
            tree_item, 3,
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
                pval(2) = _node->pval( 2) ;
  
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
        mesh_type &_mesh,
        rdel_opts &_opts
        )
    {
    /*------------------------------ initialise mesh bbox */
        real_type _plen[ +3];
        _plen[ 0] = _geom._bmax[ 0] - 
                    _geom._bmin[ 0] ;
        _plen[ 1] = _geom._bmax[ 1] - 
                    _geom._bmin[ 1] ;
        _plen[ 2] = _geom._bmax[ 2] - 
                    _geom._bmin[ 2] ;

        _plen[ 0]*= (real_type)+2.0 ;
        _plen[ 1]*= (real_type)+2.0 ;
        _plen[ 2]*= (real_type)+2.0 ;

        real_type _pmin[ +3];
        real_type _pmax[ +3];
        _pmin[ 0] = _geom._bmin[ 0] - 
                          _plen[ 0] ;
        _pmin[ 1] = _geom._bmin[ 1] - 
                          _plen[ 1] ;
        _pmin[ 2] = _geom._bmin[ 2] - 
                          _plen[ 2] ;

        _pmax[ 0] = _geom._bmax[ 0] + 
                          _plen[ 0] ;
        _pmax[ 1] = _geom._bmax[ 1] + 
                          _plen[ 1] ;
        _pmax[ 2] = _geom._bmax[ 2] + 
                          _plen[ 2] ;

        _mesh.
        _tria.push_root(_pmin, _pmax) ;

    /*------------------------------ initialise mesh root */
        _mesh.
        _tria.node(+0)->fdim() = +4 ;
        _mesh.
        _tria.node(+1)->fdim() = +4 ;
        _mesh.
        _tria.node(+2)->fdim() = +4 ;
        _mesh.
        _tria.node(+3)->fdim() = +4 ;
        
        _mesh.
        _tria.node(+0)->feat() = +0 ;
        _mesh.
        _tria.node(+1)->feat() = +0 ;
        _mesh.
        _tria.node(+2)->feat() = +0 ;
        _mesh.
        _tria.node(+3)->feat() = +0 ;
        
        _mesh.
        _tria.node(+0)->topo() = +0 ;
        _mesh.
        _tria.node(+1)->topo() = +0 ;
        _mesh.
        _tria.node(+2)->topo() = +0 ;
        _mesh.
        _tria.node(+3)->topo() = +0 ;
        
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
                (_npos)->fdim() = 0 ;
                        
            _mesh._tria.node
                (_npos)->feat() 
                    = _node->feat() ;
                    
            _mesh._tria.node
                (_npos)->topo() = 2 ;  
            
            _hint = _mesh._tria.
                node(_npos)->next() ;
            
            }
        }
                   
    }
    
    /*
    --------------------------------------------------------
     * RDEL-MAKE: build an rDT in R^3 . 
    --------------------------------------------------------
     */

    template <
    typename      init_type ,
    typename      jlog_file
             >
    __static_call
    __normal_call void_type rdel_make (
        geom_type &_geom ,
        init_type &_init ,
        mesh_type &_mesh ,
        rdel_opts &_args ,
        jlog_file &_dump
        )
    {   
    /*------------------------------ ensure deterministic */  
        std::srand( +1 ) ;

    /*------------------------------ initialise mesh obj. */
        init_mesh(_geom, _init, _mesh, _args) ;
    
        iptr_type _nbal  = +0 ;
        iptr_type _nedg  = +0 ;
        iptr_type _nfac  = +0 ;
        iptr_type _ntri  = +0 ;
        
        iptr_type _ndup  = +0 ;
    
    /*------------------------- init. for local hash obj. */
        typename 
            mesh_type::edge_list _eset (
        typename mesh_type::edge_hash(),
        typename mesh_type::edge_pred(), 
           +.8, _mesh._eset.get_alloc()) ;
           
        typename 
            mesh_type::face_list _fset (
        typename mesh_type::face_hash(),
        typename mesh_type::face_pred(), 
           +.8, _mesh._fset.get_alloc()) ;
           
        typename 
            mesh_type::tria_list _tset (
        typename mesh_type::tria_hash(),
        typename mesh_type::tria_pred(), 
           +.8, _mesh._tset.get_alloc()) ;

        iptr_list _tnew, _nnew ;
    
    /*------------------------- face in DT for rDT checks */    
        iptr_type _tpos  = +0 ;
        iptr_type _npos  = +0 ;
        
        for (auto _iter  = 
            _mesh._tria._tset.head() ; 
                  _iter != 
            _mesh._tria._tset.tend() ; 
                ++_iter, ++_tpos)
        {
            if (_iter->mark() >= +0)
            {
                _tnew. push_tail( _tpos) ;
            }
        }
        
        for (auto _iter  = 
            _mesh._tria._nset.head() ; 
                  _iter != 
            _mesh._tria._nset.tend() ; 
                ++_iter, ++_npos)
        {
            if (_iter->mark() >= +0)
            {
                _nnew. push_tail( _npos) ;
            }
        }

    /*------------------------- push alloc. for hash obj. */
        _eset._lptr.set_count (
            _tnew.count()*6 , 
        containers::loose_alloc, nullptr);
        
        _fset._lptr.set_count (
            _tnew.count()*4 , 
        containers::loose_alloc, nullptr);
        
        _tset._lptr.set_count (
            _tnew.count()*1 , 
        containers::loose_alloc, nullptr);
        
    /*------------------------- calc. voronoi-dual points */
        for( auto _iter  = _tnew.head(); 
                  _iter != _tnew.tend(); 
                ++_iter  )
        {
            tria_circ(_mesh,*_iter) ;
        }
       
    /*------------------------- test for restricted balls */
        if (_args.dims() >= 0  )
        { 
        for( auto _iter  = _nnew.head(); 
                  _iter != _nnew.tend(); 
                ++_iter  )
        {     
            char_type _kind = feat_ball;
        
            init_ball(_mesh, _geom,
                     *_iter, 
                      _kind, _nbal, 
                      _args) ;
        }
        }
    
    /*------------------------- test for restricted edges */
        if (_args.dims() >= 1  )
        { 
        for( auto _iter  = _tnew.head(); 
                  _iter != _tnew.tend(); 
                ++_iter  )
        {        
            test_edge(_mesh, _geom,
                     *_iter, 
                      _eset, _nedg, 
                      _args) ;
        }
        }
       
    /*------------------------- test for restricted edges */
        if (_args.dims() >= 2  )
        { 
        for( auto _iter  = _tnew.head(); 
                  _iter != _tnew.tend(); 
                ++_iter  )
        {        
            test_face(_mesh, _geom,
                     *_iter, 
                      _fset, 
                      _nfac, _ndup, 
                      _args) ;
        }
        }
        
    /*------------------------- test for restricted tria. */    
        if (_args.dims() >= 3  )
        { 
        bool_type _safe = true ;
        iptr_type _sign =  -1  ;
        
      //if (_nedg >= +1) _safe = false ;
        if (_ndup >= +1) _safe = false ;
        
        for( auto _iter  = _tnew.head(); 
                  _iter != _tnew.tend(); 
                ++_iter )
        {
           _sign = _safe ? _sign : -1 ;

            test_tria(_mesh, _geom,
                     *_iter, _sign, 
                      _tset, _ntri, 
                      _args) ;
        }
        }
        
        /*
        if (_args.verb() >= +2 )
        {   
    //------------------------- push rDEL memory metrics *
        
        _dump.push("\n")  ;
        _dump.push("  rDT statistics... \n") ;
        _dump.push("\n")  ;
        
        }
         */
        
        if (_args.verb() >= +2 )
        {
    /*------------------------- push rDEL scheme metrics */
        
        _dump.push("\n")  ;
        _dump.push("  rDT statistics... \n") ;
        _dump.push("\n")  ;

        _dump.push("  |rDEL-0| (node) = ") ;
        _dump.push(std::to_string (_nbal)) ;
        _dump.push("\n")  ;
        
        _dump.push("  |rDEL-1| (edge) = ") ;
        _dump.push(std::to_string (_nedg)) ;
        _dump.push("\n")  ;
        
        _dump.push("  |rDEL-2| (face) = ") ;
        _dump.push(std::to_string (_nfac)) ;
        _dump.push("\n")  ;
        
        _dump.push("  |rDEL-3| (tria) = ") ;
        _dump.push(std::to_string (_ntri)) ;
        _dump.push("\n")  ;
        _dump.push("\n")  ;
        
        }
        
        _dump.push("\n")  ;
    }
    
    } ;
    
    
    }
    
#   endif   //__RDEL_MAKE_3__ 



