MULTIFLAT RATE MODULE README

The multiflatrate module enables you to define delivery rates based on
product class. These rates will then be applied to all the products in your cart.

Furthermore you can group product type rules together to have a set of rules such as
Standard / Recorded / Next Day

For example you can have

Product Class A

with:

Base Price: 1.00
Base Unit: 1
Additional Price per unit: 0.5

and Product Class B

with:

Base Price: 1.00
Base Unit: 10
Additional Price per unit: 0.5

If in our cart we have 3 products of Class A and 12 products of class B we have:

Product Class A delivery costs:

1 product at 1.00
2 products at 0.5

Product Class B:

First 10 products delivery cost 1.00
subsequent 2 products delivery costs 0.5

Total cost:  (1 + 1) + (1 + 0.5) = 3.5


USAGE
-------
To add a multiflatrate rule you need to visit

admin/store/settings/quotes/methods/multiflatrate

and click on "Add new multiflatrate shipping method"

Enter a name and label and click save

To then add rules to the method click on Edit and you will see a new pane
underneath where you can add rules for each product class.  


# Authors/Credits
The module was financed by Homemadedigital Ltd (http://www.homemadedigital.com)
* [istos] (http://drupal.org/user/200188)
