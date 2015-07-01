var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Catraca', ip: req.ip, protocol: req.protocol });
});

module.exports = router;
